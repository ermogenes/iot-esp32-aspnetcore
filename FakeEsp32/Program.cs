var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapGet("/", () => "{ \"online\": \"true\" }");
app.MapGet("/sensorData", () => "{ \"sensor\": \"123\" }");

app.Run();
