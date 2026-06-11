function FILbrowser(config) { // entry point
    const defaultConfig = {
        new_folder : true,
        show_hidden: true,
        dir_only   : false,
        value      : '',
        callback   : function() {},
        file_types : [],
    };
    config        = {...defaultConfig, ...config};
    config.popout = POPpopoutOpen({
        title      : 'File System Browser',
        content    : FILbrowserHTML(config),
        width      : "650px",
        dismissible: false // escape is used to exit out of new folder creation so we do not want this dismissible
    });
    setTimeout(() => FILbrowserInit(config), 0); // wait for popup DOM
}

function FILbrowserInit(config) { // encapsulates all file browser logic and state
    let currentPath     = '';
    let selected        = null;
    let searchTimeout   = null;
    let volumesDropdown = null; // Track the dropdown element
    let fileData        = {};
    let history         = [];
    let historyPos      = -1;
    const root          = document.querySelector('.FIL-browser');
    if(!root) { return; }

    const header       = root.querySelector('.FIL-header');
    const controls     = root.querySelector('.FIL-controls');
    const fileList     = root.querySelector('.FIL-list');
    const cancelBtn    = root.querySelector('.FIL-cancel');
    const selectBtn    = root.querySelector('.FIL-select');
    const newFolderBtn = root.querySelector('.FIL-newfolder');
    const pathField    = root.querySelector('.FIL-pathfield');
    const searchField  = root.querySelector('.FIL-searchfield');
    const pathCopy     = root.querySelector('.FILpathCopy');
    const backBtn      = root.querySelector('.FIL-backbtn');
    const nextBtn      = root.querySelector('.FIL-nextbtn');

    function clearSearch() { searchField.value = ''; }

    function updatePathField() {
        pathField.value = selected || currentPath;
        pathField.scrollLeft = pathField.scrollWidth;
    }

    function updateNavButtons() {
        backBtn.disabled = historyPos <= 0;
        nextBtn.disabled = historyPos === -1 || historyPos >= history.length - 1;
    }

    backBtn.addEventListener('click', () => {
        if (historyPos > 0) {
            historyPos--;
            currentPath = history[historyPos];
            performNavigate(currentPath, false);
        }
        updateNavButtons();
    });

    nextBtn.addEventListener('click', () => {
        if (historyPos !== -1 && historyPos < history.length - 1) {
            historyPos++;
            currentPath = history[historyPos];
            performNavigate(currentPath, false);
        }
        updateNavButtons();
    });

    if(searchField) {
        searchField.addEventListener('input', () => {
            if(searchTimeout) { clearTimeout(searchTimeout); }
            searchTimeout = setTimeout(updateList, 250);
        });
    }

    if(newFolderBtn) {
        newFolderBtn.addEventListener('click', () => {
            if (fileList.querySelector('.FIL-newfolder-inline')) return;
            const wrapper = document.createElement('div');
            wrapper.style.display = 'flex';
            wrapper.style.flexDirection = 'column';

            const input = document.createElement('input');
            input.className = 'FIL-newfolder-inline FIL-item FIL-folder';
            input.placeholder = 'New folder name...';

            const errorDiv = document.createElement('div');
            errorDiv.className = 'FIL-error-msg';
            errorDiv.style.color = '#d73322';
            errorDiv.style.fontSize = '11px';
            errorDiv.style.marginTop = '2px';
            errorDiv.style.display = 'none';

            wrapper.appendChild(input);
            wrapper.appendChild(errorDiv);
            fileList.prepend(wrapper);
            input.focus();

            function showError(msg) {
                errorDiv.textContent   = msg;
                errorDiv.style.display = '';
            }
            function clearError() {
                errorDiv.textContent   = '';
                errorDiv.style.display = 'none';
            }
            function createDir(newFolderName) {
                if(FILnameIsInvalid(newFolderName)) {
                    return showError(`${FILnameRequirements()}. Try ${FILmakeValidFileName(newFolderName)} instead.`);
                }
                if (newFolderName && currentPath) {
                    FILmakeNewFolder(currentPath, newFolderName, (r) => {
                        if(!r.success) { return showError(r.error); }
                        performNavigate(currentPath, true)
                    });
                } else { wrapper.remove(); }
            }
            input.addEventListener('keydown', (e) => {
                clearError();
                if(e.key === 'Enter')  { createDir(input.value.trim()); }
                if(e.key === 'Escape') { wrapper.remove(); }
            });
            input.addEventListener('blur', () => createDir(input.value.trim()));
        });
    }

    if (pathCopy) { pathCopy.addEventListener('click', () => { navigator.clipboard.writeText(pathField.value); }); }

    function handleItemClick(e) {
        const item = e.target.closest('.FIL-item');
        if(!item || item.classList.contains('FIL-newfolder-inline')) { return; }
        const name = item.textContent;
        if(!fileData.hasOwnProperty(name)) { return; }
        const info = fileData[name];
        const path = FILpathSepAdd(currentPath) + name;
        if(!info.file) {
            selectItem(FILpathSepAdd(path), item);
        } else if(!config.dir_only) {
            selectItem(path, item);
        }
    }

    function handleItemDblClick(e) {
        const item = e.target.closest('.FIL-item');
        if(!item || item.classList.contains('FIL-newfolder-inline')) { return; }
        const name = item.textContent;
        if(!fileData.hasOwnProperty(name)) { return; }
        const info = fileData[name];
        const path = FILpathSepAdd(currentPath) + name;
        if(!info.file) {
            performNavigate(FILpathSepAdd(path), true);
        } else if(!config.dir_only) {
            config.callback(path);
            POPpopoutClose(config.popout);
        }
    }

    cancelBtn.addEventListener('click', () => { POPpopoutClose(config.popout); });
    selectBtn.addEventListener('click', () => {
        selected ? config.callback(selected) : config.callback(currentPath);
        POPpopoutClose(config.popout);
    });

    function setVolumes(vols) {
        if(!vols || vols.length === 0) { return; }
        if(volumesDropdown && volumesDropdown.parentNode) { volumesDropdown.parentNode.removeChild(volumesDropdown); }
        const select     = document.createElement('select');
        select.className = 'FIL-volumes-dropdown';
        select.name      = 'volumes';
        select.title     = 'Volumes';
        vols.forEach(v => {
            const option       = document.createElement('option');
            option.value       = v.replace(/[/\\]+$/, ''); // normalize (no trailing slashes)
            option.textContent = v;
            select.appendChild(option);
        });
        select.addEventListener('change', () => { performNavigate(FILpathSepAdd(select.value), true); });
        header.prepend(select);
        volumesDropdown = select;
    }

    function setBreadcrumbs(path) {
        // Remove all header children except the volumesDropdown
        Array.from(header.childNodes).forEach(node => {
            if (node !== volumesDropdown) { header.removeChild(node); }
        });

        const sep = FILsep();
        const normalizedPath = path.replace(/[/\\]+$/, ''); // Strip trailing slashes

        const volumeRootMatch = normalizedPath.match(/^([a-zA-Z]:)/);
        const volumeRoot = volumeRootMatch ? volumeRootMatch[1] : '';

        const isWindows = !!volumeRoot;
        const isPosixRoot = !isWindows && path === sep;

        // POSIX Root '/'
        if (isPosixRoot) {
            const rootCrumb = document.createElement('span');
            rootCrumb.className = 'FIL-breadcrumb';
            rootCrumb.textContent = '/';
            rootCrumb.addEventListener('click', () => performNavigate('/', true));
            header.appendChild(rootCrumb);
            updatePathField();
            return;
        }

        const parts = normalizedPath.split(/[/\\]+/).filter(Boolean);

        let cumulative = '';
        // Windows Root
        if (isWindows) {
            cumulative = volumeRoot;
            const rootCrumb = document.createElement('span');
            rootCrumb.className = 'FIL-breadcrumb';
            rootCrumb.textContent = volumeRoot;
            rootCrumb.addEventListener('click', () => performNavigate(volumeRoot + sep, true));
            header.appendChild(rootCrumb);
        }

        // POSIX Root for paths starting with '/'
        if (!isWindows && path.startsWith(sep)) {
            cumulative = sep;
            const rootCrumb = document.createElement('span');
            rootCrumb.className = 'FIL-breadcrumb';
            rootCrumb.textContent = '/';
            rootCrumb.addEventListener('click', () => performNavigate('/', true));
            header.appendChild(rootCrumb);
        }

        // Start index (skip volume in parts for Windows)
        const startIdx = (isWindows && parts[0] && parts[0].match(/^[a-zA-Z]:$/)) ? 1 : 0;

        for (let idx = startIdx; idx < parts.length; idx++) {
            header.appendChild(document.createTextNode(sep));
            cumulative = (cumulative === sep) ? sep + parts[idx] : cumulative + sep + parts[idx];

            const crumb = document.createElement('span');
            crumb.className = 'FIL-breadcrumb';
            crumb.textContent = parts[idx];
            // Capture current cumulative path in closure
            crumb.addEventListener('click', ((p) => () => performNavigate(p, true))(cumulative + sep));
            header.appendChild(crumb);
        }

        updatePathField();
    }


    function selectItem(path, item) {
        selectBtn.disabled = false;
        selected = path;
        fileList.querySelectorAll('.FIL-item').forEach(i => i.classList.remove('FIL-selected'));
        item.classList.add('FIL-selected');
        updatePathField();
    }

    function setList(files, dirOnly) {
        fileList.innerHTML = '';
        fileData = files;
        if (!files || files.length === 0) return;
        Object.entries(files).forEach(([name, info]) => {
            if(!config.show_hidden && name.startsWith('.')) { return; }
            if(dirOnly && info.file) { return; }
            if(config.file_types.length && info.file && !config.file_types.includes(info.type)) { return; }
            const item       = document.createElement('div');
            item.className   = 'FIL-item' + (info.file ? ' FIL-file' : ' FIL-folder');
            item.textContent = name;
            fileList.appendChild(item);
        });
        fileList.addEventListener('click', handleItemClick);
        fileList.addEventListener('dblclick', handleItemDblClick);
        updatePathField();
    }

    function updateList() {
        FILlist(currentPath, config.show_hidden, searchField.value, (r) => setList(r.data, config.dir_only));
        selectBtn.disabled = true;
        selected           = null;
    }

    function performNavigate(path, pushHistory=true) {
        if (pushHistory) {
            // Only push path if different to avoid dupes
            if (history.length === 0 || history[history.length - 1] !== path) {
                history.push(path);
                historyPos = history.length - 1;
            }
        }
        currentPath = path;

        // Set volume dropdown to match currentPath's volume, if possible
        if(volumesDropdown) {
            const match = Array.from(volumesDropdown.options).find(opt =>
               path.replace(/[/\\]+.*/, '') === opt.value.replace(/[/\\]+$/, '')
            );
            if(match) { volumesDropdown.value = match.value; }
        }

        clearSearch();
        setBreadcrumbs(currentPath);
        updateList();
        updatePathField();
        updateNavButtons();
    }

    FILgetVolumes((v) => {
        setVolumes((v.data.volumes || []));
        FILextractPath(config.value, (r) => {
            currentPath = r.data.path;
            performNavigate(currentPath, true);
        });
    });
}