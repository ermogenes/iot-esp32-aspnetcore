var builder = WebApplication.CreateBuilder(args);

var votacao = new Votacao();
await BuscaPorUrnas(votacao);
builder.Services.AddSingleton<Votacao>(votacao);

var app = builder.Build();

app.UseDefaultFiles();
app.UseStaticFiles();

app.MapGet("/api/placar", (Votacao votacao) => Results.Ok<Votacao>(votacao));

app.Run();

async Task BuscaPorUrnas(Votacao votacao)
{
    // Obtém endpoints do mecanismo de configuração
    var endpointUrnas = builder.Configuration["Urnas"]!.Split(",");

    // Verifica cada urna e adiciona na lista
    foreach (var endpointUrna in endpointUrnas)
    {
        using var client = new HttpClient();
        try
        {
            var result = await client.GetFromJsonAsync<InfoDTO>(endpointUrna);
            votacao.AdicionaUrna(result!.urna);
        }
        catch (System.Net.Http.HttpRequestException) { }
    }
}

class Votacao
{
    public List<Placar> Placares { get; set; } = new();

    public int Vermelhos { get => Placares.Sum<Placar>(u => u.vermelhos); }
    public int Brancos { get => Placares.Sum<Placar>(u => u.brancos); }
    public int Azuis { get => Placares.Sum<Placar>(u => u.azuis); }

    public void AdicionaUrna(string nomeUrna)
    {
        Placares.Add(new Placar { urna = nomeUrna });
    }

    public void AtualizaPlacar(Placar placarAtualizado)
    {
        var placar = Placares.Single<Placar>(
            p => p.urna == placarAtualizado.urna
        );
        placar = placarAtualizado;
    }

    public void ZeraPlacar(string urna)
    {
        var placar = Placares.Single<Placar>(p => p.urna == urna);
        placar.vermelhos = 0;
        placar.brancos = 0;
        placar.azuis = 0;
    }
}

class Placar
{
    public string urna { get; set; } = default!;
    public int vermelhos { get; set; }
    public int brancos { get; set; }
    public int azuis { get; set; }
}

record InfoDTO(bool online, string urna);
