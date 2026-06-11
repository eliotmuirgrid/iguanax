function QUILLdemoScreen(){
   // document.querySelector("body").innerHTML = "Hello World";
   document.querySelector("body").innerHTML = QUILLrenderHtml();
   // let quill = new Quill('#editor');
   
   // let ToolbarOptions = [
   //    ['bold', 'italic', 'underline', 'strike' ,'blockquote'],
   //    ['blockquote', 'code-block']
   // ]
   let ToolbarOptions = [
      ['bold', 'italic', 'underline', 'strike', 'link'],
      ['blockquote', 'code-block'],
      [{'header': [1, 2, 3, 4, 5, 6, false]}],
      [{'list': 'ordered'}, {'list': 'bullet'}],
      [{'script': 'sub'}, {'indent': '+1'}],
      [{'direction': 'rtl'}],
      [{'size': ['small', false, 'large', 'huge']}],
      ['link', 'image', 'video', 'formula'],
      [{'color': []}],
      [{'background': []}],
      [{'font': []}],
      [{'align': []}]
   ]
   let options = {
      modules: {
        toolbar: ToolbarOptions
      },
      theme: 'snow'
      // theme: 'bubble'
   }
   // let Editor = new Quill('#editor', options);
   // let Editor = new Quill('body', options);
   let Editor = new Quill('.QUILLeditor', options);

}

function QUILLrenderHtml(){
   return /*html*/`
      <!-- Create the editor container -->
      <div class="QUILLeditor"></div>
   `
}