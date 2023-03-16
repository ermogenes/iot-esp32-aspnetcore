var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

const string NomeUrna = "fake-01";

app.MapGet("/placar", () => new { nome = NomeUrna, vermelho = 0, branco = 0, azul = 0 });

app.Run();
