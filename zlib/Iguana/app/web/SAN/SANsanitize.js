const SAN_DEFAULT_CONFIG = {
    allowedTags: [
        'a', 'b', 'br', 'div', 'em', 'i', 'li', 'ol', 'p', 'span',
        'strong', 'ul', 'table', 'thead', 'tbody', 'tr', 'td', 'th'
    ],
    allowedAttributes: {
        'a': ['href', 'target', 'rel'],
        'div': ['class', 'id'],
        'span': ['class'],
        'table': ['class'],
        'td': ['colspan', 'rowspan'],
        'th': ['colspan', 'rowspan']
    },
    allowedSchemes: ['http', 'https'],
    maxDepth: 10 // maybe too low?
};

function SANsanitizeHTML(input) {
    if (!input) return '';
    const config = { ...SAN_DEFAULT_CONFIG };

    function SANsanitizeAttribute(tag, attr, value) {
        if (!config.allowedAttributes[tag]?.includes(attr)) { return null; }

        // Special handling for URLs in href attributes
        if (attr === 'href') {
            try {
                const url = new URL(value, 'https://dummy.com');
                if (!config.allowedSchemes.includes(url.protocol.slice(0, -1))) {
                    return null;
                }
                // Remove any JavaScript protocols
                if (value.toLowerCase().trim().startsWith('javascript:')) {
                    return null;
                }
            } catch { return null; }
        }

        // Encode attribute value
        return value
            .replace(/&/g, '&amp;')
            .replace(/</g, '&lt;')
            .replace(/>/g, '&gt;')
            .replace(/"/g, '&quot;')
            .replace(/'/g, '&#039;')
            .replace(/\\/g, '&#092;')
            .replace(/\//g, '&#047;');
    }

    function SANsanitizeNode(html, depth = 0) {
        if (depth > config.maxDepth) { return ''; }

        // Remove script tags and their content first
        html = html.replace(/<script\b[^<]*(?:(?!<\/script>)<[^<]*)*<\/script>/gi, '');

        const template = document.createElement('template');
        template.innerHTML = html.trim();
        
        function SANprocessNode(node) {
            if (node.nodeType === Node.TEXT_NODE) {
                return node.textContent
                    .replace(/&/g, '&amp;')
                    .replace(/</g, '&lt;')
                    .replace(/>/g, '&gt;');
            }

            // Element node
            if (node.nodeType === Node.ELEMENT_NODE) {
                const tagName = node.tagName.toLowerCase();
                
                // Skip script tags (additional safety)
                if (tagName === 'script') { return ''; }
                
                // Check if tag is allowed
                if (!config.allowedTags.includes(tagName)) {
                    return Array.from(node.childNodes)
                        .map(child => SANprocessNode(child))
                        .join('');
                }

                // Process attributes
                const attributes = Array.from(node.attributes)
                    .map(attr => {
                        const sanitizedValue = SANsanitizeAttribute(tagName, attr.name, attr.value);
                        return sanitizedValue ? `${attr.name}="${sanitizedValue}"` : null;
                    })
                    .filter(Boolean);

                // Process children
                const children = Array.from(node.childNodes)
                    .map(child => SANprocessNode(child))
                    .join('');

                const attributeString = attributes.length ? ' ' + attributes.join(' ') : '';
                return `<${tagName}${attributeString}>${children}</${tagName}>`;
            }
            return '';
        }
        return Array.from(template.content.childNodes)
            .map(node => SANprocessNode(node))
            .join('');
    }

    try {
        return SANsanitizeNode(input);
    } catch (error) {
        console.error('HTML sanitization failed:', error);
        return '';
    }
}