const POPtooltipPosition = Object.freeze({
	BOTTOM: "bottom",
	TOP: "top",
	LEFT: "left",
	RIGHT: "right",
	AUTO: "auto", // NEW
});

const POPtooltipFlowDirection = Object.freeze({
	DOWN: "down",
	UP: "up",
	CENTER: "center",
});

const POPtooltipEvent = Object.freeze({
	HOVER: "hover",
	CLICK: "click",
});

const POPtooltipType = Object.freeze({
	INFO: "info",
	ERROR: "error",
});

let POPtooltiphideTimeoutCache;

const POPactiveTooltips = new Map();

/**
 * Creates a tooltip attached to a target element
 * @param {Object} options - Configuration options
 * @param {HTMLElement} options.target - Required target element to attach tooltip to
 * @param {string} options.content - Tooltip HTML content
 * @param {keyof POPtooltipPosition} [options.position=bottom] - Placement position ('top'|'bottom'|'left'|'right'|'auto')
 * @param {keyof POPtooltipEvent} [options.event=hover] - Trigger event type
 * @param {string} [options.style] - Additional CSS classes for styling
 * @param {keyof POPtooltipType} [options.type=info] - Tooltip variant type
 * @param {boolean} [options.autoShow] - Whether to show immediately without interaction
 * @param {number} [options.hideDelay=200] - Delay before hiding (milliseconds)
 * @param {Function} [options.callback] - Callback function receiving tooltip instance (called on show)
 * @param {Function} [options.onHide] - Callback function called when tooltip is hidden/closed
 * @param {boolean} [options.keepExisting] - Skip adding a new tooltip if one is already set
 * @param {number} [options.xOffset] - Horizontal offset in pixels
 * @param {number} [options.yOffset] - Vertical offset in pixels
 * @param {('up'|'down'|'center')} [options.flowDirection=center] - Content flow direction
 * @param {boolean} [options.persist=false] - If true, tooltip will not auto-hide (stays until manually removed or toggled)
 * @returns {Object} Tooltip control object with remove() method and id property
 */
function POPtooltip(options = {}) {
	if (!options.target) {
		return null;
	}
	// Check for existing tooltips on same target with same event type
	const eventType = POPtooltipgetValidEvent(options.event);
	const existingTooltips = Array.from(POPactiveTooltips.entries()).filter(
		([id, data]) => data.config.target === options.target && data.config.event === eventType
	);

	// Remove any existing tooltips on the same target with the same event type
	if (existingTooltips.length > 0) {
		if (options.keepExisting) return null;
		existingTooltips.forEach(([id]) => POPtooltipRemove(id));
	}

	const config = {
		position: POPtooltipgetValidPosition(options.position),
		event: eventType,
		content: options.content || "",
		style: options.style || "",
		target: options.target,
		id: `POPtooltip-${Date.now()}-${Math.random().toString(36).substring(2, 5)}`,
		callback: typeof options.callback === "function" ? options.callback : null,
		onHide: typeof options.onHide === "function" ? options.onHide : null,
		autoShow: options.autoShow ?? eventType === POPtooltipEvent.CLICK,
		type: POPtooltipgetValidType(options.type),
		hideDelay: options.hideDelay || 200,
		showDelay: options.showDelay || 0, // NEW
		xOffset: options.xOffset || 0,
		yOffset: options.yOffset || 0,
		flowDirection: options.flowDirection || POPtooltipFlowDirection.CENTER,
		persist: options.persist || false,
		inContainer: !!options.target.closest(".POPpopoutCard, .TRANSmodal"),
	};

	// Store tooltip configuration for later recreation
	POPactiveTooltips.set(config.id, {
		config,
		cleanup: null,
	});

	const shouldShowImmediately =
		config.autoShow ||
		(!config.inContainer && config.event === POPtooltipEvent.HOVER && config.target.matches(":hover"));

	if (shouldShowImmediately) {
		POPtooltipShow(config.id, config.xOffset, config.yOffset);
	}

	const controllers = POPtooltipsetupEventHandlers(config);

	const tooltipData = POPactiveTooltips.get(config.id);
	tooltipData.cleanup = controllers.removeListeners;

	const returnObj = {
		remove: () => POPtooltipRemove(config.id),
		id: config.id,
	};

	return returnObj;
}

function POPtooltipcreateTooltip(config) {
	const tooltip = document.createElement("div");
	const typeClass = `POPtooltip--${config.type}`;
	// start with a neutral direction; the positioner will set the exact class
	const baseClasses = `POPtooltip ${typeClass}`;
	tooltip.className = config.style ? `${baseClasses} ${config.style}`.trim() : baseClasses;
	tooltip.id = config.id;

	let content = typeof config.content === "string" ? config.content : "";
	content = content.replace(/<br\s*\/?>/gi, '<div class="POPtooltip-separator"></div>');
	tooltip.innerHTML = content;

	document.body.appendChild(tooltip);
	POPtooltippositionElement(
		tooltip,
		config.target,
		config.position, // may be 'auto'
		config.xOffset,
		config.yOffset,
		config.flowDirection
	);

	return tooltip;
}


function POPtooltippositionElement(tooltip, target, position, xOffset, yOffset, flowDirection) {
	let ARROW_OFFSET = 22;

	const wasHidden = tooltip.style.display === "none";
	if (wasHidden) {
		tooltip.style.visibility = "hidden";
		tooltip.style.display = "block";
	}

	const targetRect = target.getBoundingClientRect();
	let tooltipRect = tooltip.getBoundingClientRect();

	// Resolve 'auto' -> 'top' or 'bottom'
	let finalPosition = position;
	if (position === POPtooltipPosition.AUTO) {
		finalPosition = POPtooltipResolveVertical(tooltipRect, targetRect, ARROW_OFFSET);
	}
	if (finalPosition === POPtooltipPosition.TOP) {
		ARROW_OFFSET = -6;
	}

	const baseTop = targetRect.top + window.scrollY;
	const baseLeft = targetRect.left + window.scrollX;

	let top, left;

	const positionFlow = (flowDirection) => {
		if (flowDirection === POPtooltipFlowDirection.DOWN) {
			top = baseTop - 5;
			tooltip.classList.add("POPtooltip-flowDown");
			tooltip.classList.remove("POPtooltip-flowUp");
		} else if (flowDirection === POPtooltipFlowDirection.UP) {
			top = baseTop + targetRect.height - tooltipRect.height + 10;
			tooltip.classList.add("POPtooltip-flowUp");
			tooltip.classList.remove("POPtooltip-flowDown");
		} else {
			top = baseTop + (targetRect.height / 2 - tooltipRect.height / 2);
			tooltip.classList.remove("POPtooltip-flowUp", "POPtooltip-flowDown");
		}
	};

	switch (finalPosition) {
		case POPtooltipPosition.BOTTOM:
			top = baseTop + targetRect.height + ARROW_OFFSET;
			left = baseLeft + (targetRect.width / 2 - tooltipRect.width / 2);
			break;
		case POPtooltipPosition.TOP:
			top = baseTop - tooltipRect.height - ARROW_OFFSET;
			left = baseLeft + (targetRect.width / 2 - tooltipRect.width / 2);
			break;
		case POPtooltipPosition.LEFT:
			positionFlow(flowDirection);
			left = baseLeft - tooltipRect.width - 20;
			break;
		case POPtooltipPosition.RIGHT:
			positionFlow(flowDirection);
			left = baseLeft + targetRect.width + 20;
			break;
	}

	tooltip.style.top = `${top + (yOffset || 0)}px`;
	tooltip.style.left = `${left + (xOffset || 0)}px`;

	tooltip.classList.remove("POPtooltip-top", "POPtooltip-bottom", "POPtooltip-left", "POPtooltip-right");
	tooltip.classList.add(`POPtooltip-${finalPosition}`);

	if (wasHidden) {
		tooltip.style.visibility = "";
		tooltip.style.display = "none";
	}
}

// Set up event handlers for showing/hiding tooltip
function POPtooltipsetupEventHandlers(config) {
	const isHoverMode = config.event === POPtooltipEvent.HOVER;
	let hideTimeout;
	let showTimeout; // NEW

	const showTooltip = () => {
		clearTimeout(hideTimeout);
		clearTimeout(showTimeout); // NEW: don't stack show timers

		// If already visible, no-op
		const tooltipData = POPactiveTooltips.get(config.id);
		if (tooltipData?.tooltip) return;

		POPtooltipShow(config.id, config.xOffset, config.yOffset);
		if (config.callback) {
			config.callback();
		}
	};

	const scheduleShowTooltip = () => {
		clearTimeout(showTimeout);
		const delay = config.showDelay || 0;

		if (delay <= 0) {
			showTooltip();
			return;
		}

		showTimeout = setTimeout(() => {
			showTooltip();
		}, delay);
	};

	const hideTooltip = () => {
		// NEW: respect persist — never auto-hide if persistent
		if (config.persist) return;

		clearTimeout(showTimeout); // NEW: cancel pending show if leaving early

		hideTimeout = setTimeout(() => {
			// Only hide if not hovering over tooltip itself
			const tooltipData = POPactiveTooltips.get(config.id);
			if (tooltipData?.tooltip?.matches(":hover")) return;
			POPtooltipHide(config.id);
		}, config.hideDelay || 200);
	};

	// Hover event handler
	const handleHover = (e) => {
		if (!isHoverMode) return;
		if (e.type === "mouseenter") {
			scheduleShowTooltip(); // NEW: delayed show
		} else if (e.type === "mouseleave") {
			hideTooltip();
		}
	};

	const handleTargetClick = () => {
		if (isHoverMode) return;

		const tooltipData = POPactiveTooltips.get(config.id);
		const isVisible = tooltipData && tooltipData.tooltip;

		// Click toggles visibility even if persistent
		if (isVisible) {
			POPtooltipHide(config.id);
		} else {
			showTooltip();
		}
	};

	// Document click handler for closing click-triggered tooltips
	const handleDocumentClick = (e) => {
		if (isHoverMode) return;

		const tooltipData = POPactiveTooltips.get(config.id);
		if (!tooltipData || !tooltipData.tooltip) return;

		// NEW: skip outside-click hiding when persistent
		if (!config.persist && !config.target.contains(e.target) && !tooltipData.tooltip.contains(e.target)) {
			POPtooltipHide(config.id);
		}
	};

	// Attach event listeners based on mode
	if (isHoverMode) {
		config.target.addEventListener("mouseenter", handleHover);
		config.target.addEventListener("mouseleave", handleHover);
	} else {
		config.target.addEventListener("click", handleTargetClick);
		document.addEventListener("click", handleDocumentClick);
	}

	const removeListeners = () => {
		clearTimeout(showTimeout); // NEW
		clearTimeout(hideTimeout);

		if (isHoverMode) {
			config.target.removeEventListener("mouseenter", handleHover);
			config.target.removeEventListener("mouseleave", handleHover);
		} else {
			config.target.removeEventListener("click", handleTargetClick);
			document.removeEventListener("click", handleDocumentClick);
		}
	};

	return {
		removeListeners,
	};
}

// Helper function to show a tooltip by ID
function POPtooltipShow(id, xOffset, yOffset) {
	const tooltipData = POPactiveTooltips.get(id);
	if (!tooltipData) return;

	// If tooltip already exists, no need to create a new one
	if (tooltipData.tooltip) return;

	const tooltip = POPtooltipcreateTooltip(tooltipData.config);

	// Set up hover events if needed
	if (tooltipData.config.event === POPtooltipEvent.HOVER) {
		tooltip.addEventListener("mouseenter", () => {
			clearTimeout(POPtooltiphideTimeoutCache);
		});

		tooltip.addEventListener("mouseleave", () => {
			// respect persist on hover leave
			if (tooltipData.config.persist) return;

			const target = tooltipData.config.target;

			// NEW: if the user is still on the target, keep tooltip open
			if (target && target.matches && target.matches(":hover")) return;

			POPtooltiphideTimeoutCache = setTimeout(() => {
				POPtooltipHide(id);
			}, tooltipData.config.hideDelay);
		});

	}

	tooltipData.tooltip = tooltip;
	POPtooltippositionElement(
		tooltip,
		tooltipData.config.target,
		tooltipData.config.position,
		xOffset,
		yOffset,
		tooltipData.config.flowDirection
	);

	// Execute callback after tooltip is positioned and visible
	if (tooltipData.config.callback) {
		tooltipData.config.callback(tooltip);
	}
}

function POPtooltipHide(id) {
	const tooltipData = POPactiveTooltips.get(id);
	if (!tooltipData || !tooltipData.tooltip) return;

	// onHide hook (fires when the tooltip is actually closed)
	if (tooltipData.config.onHide) {
		tooltipData.config.onHide(tooltipData.tooltip, tooltipData.config);
	}

	// NOTE: We allow manual hides even if persistent (only auto-hides are blocked)
	if (tooltipData.tooltip.parentNode) {
		tooltipData.tooltip.parentNode.removeChild(tooltipData.tooltip);
	}
	tooltipData.tooltip = null;
}

function POPtooltipRemove(id) {
	const tooltipData = POPactiveTooltips.get(id);
	if (!tooltipData) return false;

	// If visible, hide first so onHide fires consistently
	POPtooltipHide(id);

	// Remove listeners
	if (typeof tooltipData.cleanup === "function") {
		tooltipData.cleanup();
	}

	// Finally remove config
	POPactiveTooltips.delete(id);
	return true;
}

function POPtooltipRemoveByClass(className) {
	POPactiveTooltips.forEach((data, id) => {
		if (data.config.style === className) {
			POPtooltipRemove(id);
		}
	});
}

function POPtooltipResolveVertical(tooltipRect, targetRect, arrowOffset) {
	const spaceAbove = targetRect.top;
	const spaceBelow = window.innerHeight - targetRect.bottom;
	const required = tooltipRect.height + arrowOffset;

	if (spaceBelow >= required) return POPtooltipPosition.BOTTOM;
	if (spaceAbove >= required) return POPtooltipPosition.TOP;
	return spaceBelow >= spaceAbove ? POPtooltipPosition.BOTTOM : POPtooltipPosition.TOP;
}

function POPtooltipgetValidPosition(inputPosition) {
	return Object.values(POPtooltipPosition).includes(inputPosition) ? inputPosition : POPtooltipPosition.BOTTOM;
}

function POPtooltipgetValidEvent(inputEvent) {
	return Object.values(POPtooltipEvent).includes(inputEvent) ? inputEvent : POPtooltipEvent.HOVER;
}

function POPtooltipgetValidType(inputType) {
	return Object.values(POPtooltipType).includes(inputType) ? inputType : POPtooltipType.INFO;
}
