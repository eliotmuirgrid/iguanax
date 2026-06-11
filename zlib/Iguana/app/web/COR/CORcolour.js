/* ---------- tiny COR helpers ---------- */
const CORclamp = (v, min = 0, max = 1) => Math.min(max, Math.max(min, v));

const CORhexToRgb = (hex) => {
  const int = parseInt(hex.slice(1), 16);
  return { r: (int >> 16) & 255, g: (int >> 8) & 255, b: int & 255 };
};

const CORrgbToHex = ({ r, g, b }) =>
  '#' +
  [r, g, b]
    .map((c) => c.toString(16).padStart(2, '0'))
    .join('')
    .toLowerCase();

const CORrgbToHsl = ({ r, g, b }) => {
  r /= 255; g /= 255; b /= 255;
  const max = Math.max(r, g, b), min = Math.min(r, g, b);
  const l = (max + min) / 2;

  if (max === min) return { h: 0, s: 0, l };

  const d = max - min;
  const s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
  const h =
    max === r
      ? ((g - b) / d + (g < b ? 6 : 0))
      : max === g
      ? (b - r) / d + 2
      : (r - g) / d + 4;

  return { h: h * 60, s, l };
};

const CORhslToRgb = ({ h, s, l }) => {
  const c = (1 - Math.abs(2 * l - 1)) * s;
  const x = c * (1 - Math.abs(((h / 60) % 2) - 1));
  const m = l - c / 2;
  const [r1, g1, b1] =
    h < 60
      ? [c, x, 0]
      : h < 120
      ? [x, c, 0]
      : h < 180
      ? [0, c, x]
      : h < 240
      ? [0, x, c]
      : h < 300
      ? [x, 0, c]
      : [c, 0, x];

  return {
    r: Math.round((r1 + m) * 255),
    g: Math.round((g1 + m) * 255),
    b: Math.round((b1 + m) * 255),
  };
};

/**
 * -------- CORlightenColour --------
 *
 * @param {string} colour   '#rrggbb' hex (e.g. '#00609f')
 * @param {number} amount   0–1  how far toward white (default 0.753)
 * @param {number} desat    0–1  how much to desaturate (default 0.32)
 * @param {number} hueShift degrees to tweak hue (default -2)
 * @returns {string}        lighter shade as hex, or 'b3ddf5' on error
 */
function CORlightenColour(
  colour,
  amount = 0.753,
  desat = 0.32,
  hueShift = -2
) {
  const DEFAULT = 'b3ddf5';

  try {
    if (!/^#([0-9a-f]{6})$/i.test(colour)) throw new Error();

    const hsl = CORrgbToHsl(CORhexToRgb(colour));

    hsl.l = CORclamp(hsl.l + (1 - hsl.l) * amount);       // lighten
    hsl.s = CORclamp(hsl.s * (1 - amount * desat));        // desaturate
    hsl.h = (hsl.h + hueShift + 360) % 360;                // hue tweak

    return CORrgbToHex(CORhslToRgb(hsl));
  } catch {
    return DEFAULT; // graceful fallback
  }
}
