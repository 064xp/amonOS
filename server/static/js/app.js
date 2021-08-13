const input = document.querySelector("#prompt-input");
const outputBox = document.querySelector("#output-box");

input.addEventListener("keyup", e => {
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
    const command = e.target.value;

    e.target.value = "";
    axios
      .post("/request", { command: command })
      .catch(error => {
        console.log(error);
      })
      .then(res => {
        console.log(res.data);
        // const user = "root";
        // const cwd = "/";
        // const sym = user === "root" ? "#" : "$";
        // const output = "test output";
        // appendHTML(outputBox, template, [user, cwd, sym, command, output]);
      });
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

const focusInput = e => {
  setTimeout(() => {
    input.focus();
  }, 100);
};

// Global Event listeneVr
document.addEventListener("keyup", focusInput);
