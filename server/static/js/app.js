const input = document.querySelector("#prompt-input");
const outputBox = document.querySelector("#output-box");
const cwdText = document.querySelector("#prompt-cwd-input");

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
    if (command == "clear") {
      outputBox.innerHTML = "";
      e.target.value = "";
      return;
    }

    axios
      .post("/request", { command: command })
      .catch(error => {
        console.log(error);
      })
      .then(res => {
        const user = res.data.user;
        const cwd = res.data.cwd;
        const sym = user === "root" ? "#" : "$";
        const output = res.data.buffer;
        const container = document.querySelector(".container");

        appendHTML(outputBox, template, [
          user,
          cwdText.innerHTML,
          sym,
          command,
          output
        ]);
        e.target.value = "";
        cwdText.innerText = cwd;

        container.scrollTop = container.scrollHeight;
      });
  }
});

const appendHTML = (parent, str, values) => {
  values.forEach((value, index) => {
    let valueAdjusted = value.replaceAll("\n", "<br />");
    var findStr = "{{" + (index + 1) + "}}";
    str = str.replaceAll(findStr, valueAdjusted);
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
