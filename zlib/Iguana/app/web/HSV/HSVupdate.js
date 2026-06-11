function HSVtargetToItemData(target) {
    if (!target) return;
    const wrapper = target.closest('.HSVdiffWrapper');
    if (!wrapper) return;
    const wrappers = Array.from(document.querySelectorAll('.HSVdiffWrapper'));
    const index = wrappers.indexOf(wrapper);
    return index;
}