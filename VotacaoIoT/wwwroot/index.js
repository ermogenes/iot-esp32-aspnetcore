document.addEventListener("DOMContentLoaded", async () => {
  await atualizaPlacarGeral(); // imediatamente
  await setInterval(atualizaPlacarGeral, 2000); // repete a cada 2s
});

const atualizaPlacarGeral = async () => {
  try {
    const respostaHttp = await fetch("/placarGeral");

    document.getElementById("placar").classList.remove("inativo");

    const placarGeral = await respostaHttp.json();

    document.getElementById("vermelho").innerText = placarGeral.vermelho;
    document.getElementById("branco").innerText = placarGeral.branco;
    document.getElementById("azul").innerText = placarGeral.azul;
  } catch (e) {
    document.getElementById("placar").classList.add("inativo");
  }
};
