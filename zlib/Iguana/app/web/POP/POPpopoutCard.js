// Dialog library
// Usage:
// let Card = POPpopoutOpen({title : "A dialog", content : "Some content", callback : function() { console.log("Clean up"); } });
// POPpopoutClose(Card);

const POPactivePopouts = [];

/**
 * Creates and opens a popout card
 * @param {Object} T - Configuration options
 * @returns {Object} Popout control object
 */
function POPpopoutOpen(T) {
	const options = {
		title: T?.title || '',
		content: T?.content || '',
		id: T?.Id || POPgenerateUniqueId(),
		dismissible: T?.dismissible ?? true,
		zIndex: T?.Zindex || 999,
		className: T?.className || '',
		callback: T?.callback,
		
		top: T?.top,
		left: T?.left,
		width: T?.width,
		minWidth: T?.minWidth,
		maxWidth: T?.maxWidth,
		height: T?.height,
		maxHeight: T?.maxHeight,
		minHeight: T?.minHeight
	};
	
	const popoutElement = POPcreatePopoutElement(options);
	
	document.body.appendChild(popoutElement);
	
	if (options.dismissible === false) {
		const closeButton = popoutElement.querySelector(".POPpopoutCard--close");
		if (closeButton) closeButton.style.display = 'none';
	}
	
	POPsetupEventListeners(popoutElement, options);
	
	const popoutObj = { 
		...options, 
		element: popoutElement
	};
	POPactivePopouts.push(popoutObj);
	
	if (POPactivePopouts.length === 1) {
		document.addEventListener('keydown', POPhandleEscapeKey);
	}
	
	return popoutObj;
}

function POPconvertToCSSProperty(camelCase) {
  return camelCase.replace(/[A-Z]/g, m => "-" + m.toLowerCase());
}

function POPcreatePopoutElement(options) {
	const popout = document.createElement('div');
	popout.className = `POPpopoutCard ${options.id} POPpopoutCard--anchor`;
	
	const styles = {
		width: options.width,
		minWidth: options.minWidth,
		maxWidth: options.maxWidth,
		height: options.height,
		minHeight: options.minHeight,
		maxHeight: options.maxHeight,
		left: options.left,
		top: options.top,
		zIndex: options.zIndex
	};
	
	const styleString = Object.entries(styles)
		.filter(([_, value]) => value !== undefined)
		.map(([key, value]) => `${POPconvertToCSSProperty(key)}:${value}`)
		.join(';');
	
	popout.innerHTML = `
		<div class="POPpopoutCard-overlay" tabindex="-1" style="z-index:${options.zIndex - 1}"></div>
		<div class="POPpopoutCard-wrapper ${options.className}" style="${styleString};">
			<div class="POPpopoutCard-header">
				<div class="POPpopoutCard--close" label="Close Card" id="closePopoutCard${options.id}"></div>
				<div class="POPpopoutCard-title">
					<h3>${options.title}</h3>
				</div>
			</div>
			<div class="POPpopoutCard-content">
				${options.content}
			</div>
		</div>
	`;
	
	return popout;
}

function POPsetupEventListeners(popoutElement, options) {
	const closeButton = popoutElement.querySelector('.POPpopoutCard--close');
	if (closeButton) {
		closeButton.addEventListener('click', () => {
			POPpopoutClose({ id: options.id, callback: options.callback });
		});
	}
	
	const cancelButtons = popoutElement.querySelectorAll('.POPcancelAction');
	cancelButtons.forEach(button => {
		button.addEventListener('click', () => {
			POPpopoutClose({ id: options.id, callback: options.callback });
		});
	});
	
	if (options.dismissible) {
		const overlay = popoutElement.querySelector('.POPpopoutCard-overlay');
		if (overlay) {
			overlay.addEventListener('mousedown', (e) => {
				if (e.button !== 2) { // Not right-click
					POPpopoutClose({ id: options.id, callback: options.callback });
				}
			});
		}
	}
}

function POPpopoutClose(T) {
	if (!T || !T.id) return false;
	
	// Find popout in active list
	const popoutIndex = POPactivePopouts.findIndex(p => p.id === T.id);
	if (popoutIndex === -1) return false;
	
	// Get the popout from active list
	const popout = POPactivePopouts[popoutIndex];
	
	// Remove from DOM
	if (popout.element && popout.element.parentNode) {
		popout.element.parentNode.removeChild(popout.element);
	} else {
		// Fallback to querySelector if element reference is missing
		const popoutElement = document.querySelector(`.${T.Id}`);
		if (popoutElement) {
			popoutElement.remove();
		}
	}
	// Remove from active list
	POPactivePopouts.splice(popoutIndex, 1);
	if (POPactivePopouts.length === 0) {
		document.removeEventListener('keydown', POPhandleEscapeKey);
	}
	if (T.callback) {
		setTimeout(T.callback, 100);
	}
	return true;
}

function POPpopoutRemoveAll() {
	[...POPactivePopouts].reverse().forEach(popout => {
		POPpopoutClose({ Id: popout.id, callback: popout.callback });
	});
	
	document.querySelectorAll('.POPpopoutCard').forEach(element => {
		if (element && element.parentNode) {
			element.parentNode.removeChild(element);
		}
	});
	
	POPactivePopouts.length = 0;
	document.removeEventListener('keydown', POPhandleEscapeKey);
}

function POPhandleEscapeKey(event) {
	if (event.key !== "Escape" || POPactivePopouts.length === 0) return;
	
	const lastPopout = POPactivePopouts[POPactivePopouts.length - 1];
	if (lastPopout.dismissible === false) return;
	
	POPpopoutClose(lastPopout);
}

function POPgenerateUniqueId() {
	return `popout-${Date.now()}-${Math.random().toString(36).substring(2, 9)}`;
}
