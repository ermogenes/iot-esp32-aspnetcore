document.addEventListener('DOMContentLoaded', async () => {
    alert("ok");
    const response = await fetch('/data');
    const result = await response.json();
    console.log(result);
    document.getElementById('data-from-device').innerText = result.sensor;
});