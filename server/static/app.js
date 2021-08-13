const usuario = document.querySelector("#usuario");
const cmd = document.querySelector("#cmd");
const btn_cargar = document.querySelector("#btn-cargar");

btn_cargar.addEventListener("click", event => {
  event.preventDefault();
  axios
    .post("/request", { user: usuario.value, command: cmd.value })
    .catch(error => {
      console.log(error);
    })
    .then(res => {
      console.log(res.data);
    });
});
