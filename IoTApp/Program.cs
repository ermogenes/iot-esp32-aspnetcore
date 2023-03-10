var builder = WebApplication.CreateBuilder(args);

var app = builder.Build();

app.UseDefaultFiles();
app.UseStaticFiles();

/// app.MapGet("/", () => "Hello World!");

app.MapGet("/data", async () => {
    using var client = new HttpClient();

    var result = await client.GetFromJsonAsync<EspDataDTO>("http://localhost:5000/sensorData");
    return Results.Ok(result);
});

app.Run();

record EspDataDTO(string sensor);