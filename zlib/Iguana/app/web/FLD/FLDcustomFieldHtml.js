
function FLDcustomizeStringFieldHtml(Field){
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  selected >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    >File</option>
            <option value="folder"  >Folder</option>
            <option value="list"    >List</option>
            <option value="number"  >Number</option>
            <option value="password">Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
         <input class="FORMinput FLDvalueEdit" type="text" placeholder="Default value" value="${ESChtmlEscape(Field.default)}">
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizeNumberFieldHtml(Field) {
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    >File</option>
            <option value="folder"  >Folder</option>
            <option value="list"    >List</option>
            <option value="number" selected>Number</option>
            <option value="password">Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
         <input class="FORMinput FLDvalueEdit" type="text" placeholder="Default value" value="${ESChtmlEscape(Field.default)}">
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizeBoolFieldHtml(Field) {
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool" selected>Bool</option>
            <option value="file"    >File</option>
            <option value="folder"  >Folder</option>
            <option value="list"    >List</option>
            <option value="number"  >Number</option>
            <option value="password">Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
      <select class="FORMselectBox FLDvalueEdit">
         <option value="true"  ${ Field.default == "true" ? "selected" : ""}>true</option>  <!-- IX-3061 Can't just do Field.default as it is a string, and any non-empty string evaluates to true-->
         <option value="false" ${ Field.default == "false" ? "selected" : ""}>false</option>
      </select>
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizePortFieldHtml(Field) {
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    >File</option>
            <option value="folder"  >Folder</option>
            <option value="list"    >List</option>
            <option value="number"  >Number</option>
            <option value="password">Password</option>
            <option value="port"    selected>Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
         <input class="FORMinput FLDvalueEdit" type="text" placeholder="Default value" value="${ESChtmlEscape(Field.default)}">
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizePasswordFieldHtml(Field) {
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    >File</option>
            <option value="folder"  >Folder</option>
            <option value="list"    >List</option>
            <option value="number"  >Number</option>
            <option value="password" selected>Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
         <input class="FORMinput FLDvalueEdit" type="text" placeholder="Passwords cannot have defaults" value="" disabled>
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizeFileFieldHtml(Field){
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    selected>File</option>
            <option value="folder" >Folder</option>
            <option value="list"    >List</option>
            <option value="number"  >Number</option>
            <option value="password">Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
         <input class="FORMinput FLDvalueEdit" type="text" placeholder="Default value" value="${ESChtmlEscape(Field.default)}">
         <div class="FLDfileBrowseCustom BUTTONcancel BUTTONstandard">Browse</div>
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizeFolderFieldHtml(Field){
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    >File</option>
            <option value="folder" selected>Folder</option>
            <option value="list"    >List</option>
            <option value="number"  >Number</option>
            <option value="password">Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableCell FLDvalueEditFormat">
         <input class="FORMinput FLDvalueEdit" type="text" placeholder="Default value" value="${ESChtmlEscape(Field.default)}">
         <div class="FLDfolderBrowseCustom BUTTONcancel BUTTONstandard">Browse</div>
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}

function FLDcustomizeListFieldHtml(Field){
   return /*html*/`
   <div class="FLDfieldTableRow">
      <div class="FLDtableCell FLDmoveRow">⋮⋮</div>
      <div class="FLDtableCell"><input class="FORMinput FLDnameEdit" type="text" placeholder="Name" value="${ESChtmlEscape(Field.name)}"></div>
      <div class="FLDtableCell">
         <select class="FORMselectBox FLDtypeSelect" value="${Field.type}">
            <option value="default" disabled>--Type--</option>
            <option value="string"  >String</option>
            <option value="bool"    >Bool</option>
            <option value="file"    >File</option>
            <option value="folder"  >Folder</option>
            <option value="list" selected>List</option>
            <option value="number"  >Number</option>
            <option value="password">Password</option>
            <option value="port"    >Port</option>
         </select>
      </div>
      <div class="FLDtableLastColumn">
      <div class="FLDtableCell FLDvalueEditFormat">
            <input class="FORMinput FLDvalueEdit" type="text" placeholder="Defaults to first" value="${ESChtmlEscape(Field.default)}" disabled>
         </div>
         <div class="FLDtableCell FLDvalueEditFormat">
            <input class="FORMinput FLDvoptionsEdit" type="text" placeholder="List options (CSV)" value="${ESChtmlEscape(Field.options)}">
         </div>
      </div>
      <div class="FLDtableCell">
         <input class="FORMinput FLDdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Field.description || '')}">
      </div>
      <div class="FLDtableCell">
         <div class="BUTTONcancel FLDdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>
   `;
}
