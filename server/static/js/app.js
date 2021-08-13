const input = document.querySelector("#prompt-input");
const outputBox = document.querySelector("#output-box");

const focusInput = () => {
  setTimeout(() => {
    input.focus();
  }, 100);
};

input.addEventListener("keypress", e => {
  const code = e.keyCode ? e.keyCode : e.which;
  const template = `
  <p class="prompt-text">
    <span class="prompt-user">{{1}}</span
    >@amonOS:<span class="prompt-cwd" >{{2}}</span
    ><span class="prompt-sym" id="prompt-sym-input">{{3}}</span>
    <span class="input-text">{{4}}</span>
    <p class="output-text">{{5}}</p>
  </p>
  `;
  if (code == 13) {
    const input = e.target.value;
    const user = "root";
    const cwd = "/";
    const sym = user === "root" ? "#" : "$";
    const output = "test output";

    e.target.value = "";
    appendHTML(outputBox, template, [user, cwd, sym, input, output]);
    // axios
    //   .post("/request", { user: usuario.value, command: cmd.value })
    //   .catch(error => {
    //     console.log(error);
    //   })
    //   .then(res => {
    //     console.log(res.data);
    //   });
  }
});

const appendHTML = (parent, str, values) => {
  values.forEach((valor, index) => {
    var findStr = "{{" + (index + 1) + "}}";
    str = str.replaceAll(findStr, escapeHTML(valor));
  });
  parent.innerHTML += str;
  return str;
};

const escapeHTML = str => {
  return new Option(str).innerHTML;
};

// Global Event listener
document.addEventListener("click", focusInput);
input.addEventListener("blur", focusInput);
