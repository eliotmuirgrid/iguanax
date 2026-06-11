function THMcacheRadio() {
   const SelectedOption = document.querySelector('input[name="THMbrowser"]:checked').value;
   const username = SESScache?.username;
   if (SelectedOption === "dark" || SelectedOption === "light" || SelectedOption === "browser") {
      THMsetUserTheme(username, SelectedOption);
   }
   CORthemeCheckAndApply();
   MODELupdate('CORmodel');
}
