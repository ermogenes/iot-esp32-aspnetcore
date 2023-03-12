var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

const string NomeUrna = "fake-01";

app.MapGet("/", () => new { online = true, urna = NomeUrna });
app.MapGet("/placar", () => new { urna = NomeUrna, vermelhos = 0, brancos = 0, azuis = 0 });
app.MapGet("/reiniciar", () => { return Results.Ok(); });

app.Run();
