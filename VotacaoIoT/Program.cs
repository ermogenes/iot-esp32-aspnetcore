var builder = WebApplication.CreateBuilder(args);

builder.Services.AddHttpClient();
builder.Services.AddSingleton<CentralDeApuracao>();
builder.Services.AddHostedService<TimerAtualizacao>();

var app = builder.Build();

app.UseDefaultFiles();
app.UseStaticFiles();

app.MapGet("/placarGeral", (CentralDeApuracao apuracao) =>
    Results.Ok<PlacarDTO>(apuracao.PlacarGeral)
);

app.Run();

public class PlacarDTO
{
    public string nome { get; set; } = default!;
    public int vermelho { get; set; }
    public int branco { get; set; }
    public int azul { get; set; }
}

public class CentralDeApuracao
{
    private readonly ILogger<CentralDeApuracao> _logger;
    private readonly IHttpClientFactory _http;
    private List<string> _endpointsUrnas { get; set; } = default!;
    private List<PlacarDTO> _placarUrnas { get; set; } = new();
    public PlacarDTO PlacarGeral { get; private set; } = new();
    public CentralDeApuracao(IHttpClientFactory http, ILogger<CentralDeApuracao> logger, IConfiguration config)
    {
        _http = http;
        _logger = logger;
        _endpointsUrnas = config["EndpointsUrnas"]!.Split(",").ToList();
    }
    public async Task AtualizaPlacarGeral()
    {
        using var client = _http.CreateClient();

        // Atualiza o placar de cada urna configurada
        foreach (var url in _endpointsUrnas)
        {
            try
            {
                // Obtém os placares atuais na urna e no servidor
                var placarUrna = await client.GetFromJsonAsync<PlacarDTO>(url);
                var placarAnterior = _placarUrnas.SingleOrDefault<PlacarDTO>(
                    p => p.nome == placarUrna!.nome
                );

                // Cria no primeiro acesso, atualiza nos demais
                if (placarAnterior == null)
                    _placarUrnas.Add(placarUrna!);
                else
                {
                    placarAnterior.vermelho = placarUrna!.vermelho;
                    placarAnterior.branco = placarUrna!.branco;
                    placarAnterior.azul = placarUrna!.azul;
                }
            }
            // Ignora os erros de HTTP
            catch (System.Net.Http.HttpRequestException)
            {
                _logger.LogError($"Erro ao acessar {url}");
            }
        }

        // Calcula soma dos placares de todas as urnas online
        PlacarGeral.vermelho = _placarUrnas.Sum(p => p.vermelho);
        PlacarGeral.branco = _placarUrnas.Sum(p => p.branco);
        PlacarGeral.azul = _placarUrnas.Sum(p => p.azul);

        _logger.LogInformation($"Placares atualizados em {DateTime.Now}");
    }
}

class TimerAtualizacao : IHostedService
{
    private readonly CentralDeApuracao _apuracao;
    public TimerAtualizacao(CentralDeApuracao apuracao) =>
        _apuracao = apuracao;
    public Task StartAsync(CancellationToken ct)
    {
        // Executa "AtualizaPlacarGeral" cada 1 segundo
        new Timer(ExecutaAcao!, null, TimeSpan.Zero, TimeSpan.FromSeconds(1));
        return Task.CompletedTask;
    }
    private async void ExecutaAcao(object state) =>
        await _apuracao.AtualizaPlacarGeral();

    public Task StopAsync(CancellationToken ct) =>
        Task.CompletedTask;
}
