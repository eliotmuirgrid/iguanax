function UTLdisableButtonEl(el) {
	el.classList.remove("BUTTONaction");
	el.classList.add("BUTTONdisabled");
	el.style.setProperty("disabled", true);
}

function UTLenableButtonEl(el) {
	el.classList.remove("BUTTONdisabled");
	el.classList.add("BUTTONaction");
	el.style.setProperty("disabled", false);
}