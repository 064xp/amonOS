const input = document.querySelector("#prompt-input");
const outputBox = document.querySelector("#output-box");
const cwdText = document.querySelector("#prompt-cwd-input");
const authenticatedPrompt = document.querySelector(".authenticated-prompt");
const unauthenticatedPrompt = document.querySelector(".unauthenticated-prompt");
const promptUserInput = document.querySelector("#prompt-user-input");
const promptSymInput = document.querySelector("#prompt-sym-input");
const promptCwdInput = document.querySelector("#prompt-cwd-input");

let isAuthenticated = false;
let prevUser = "";
let prevCWD;

input.addEventListener("keyup", e => {
  const code = e.keyCode ? e.keyCode : e.which;
  const template = isAuthenticated
    ? `
  <p class="prompt-text">
    <span class="prompt-user">{{1}}</span
    >@amonOS:<span class="prompt-cwd" >{{2}}</span
    ><span class="prompt-sym" id="prompt-sym-input">{{3}}</span>
    <span class="input-text">{{4}}</span>
    <pre class="output-text">{{5}}</pre>
  </p>
  `
    : `
  <p class="prompt-text">
    <span class="prompt-sym" id="prompt-sym-input">> </span>
    <span class="input-text">{{4}}</span>
    <pre class="output-text">{{5}}</pre>
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

        if (user === "") isAuthenticated = false;
        else isAuthenticated = true;

        if (isAuthenticated) {
          authenticatedPrompt.classList.remove("hide");
          unauthenticatedPrompt.classList.add("hide");

          promptUserInput.innerText = user;
          promptCwdInput.innerText = cwd;
          promptSymInput.innerText = sym;
        } else {
          authenticatedPrompt.classList.add("hide");
          unauthenticatedPrompt.classList.remove("hide");
        }

        appendHTML(outputBox, template, [
          prevUser,
          prevCWD,
          prevUser === "root" ? "#" : "$",
          command,
          output
        ]);
        e.target.value = "";
        cwdText.innerText = cwd;

        prevUser = user;
        prevCWD = cwd;
        container.scrollTop = container.scrollHeight;
      });
  }
});

const appendHTML = (parent, str, values) => {
  values.forEach((value, index) => {
    var findStr = "{{" + (index + 1) + "}}";
    str = str.replaceAll(findStr, value);
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
