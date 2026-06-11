/* ==========================================================================
   PRE Health
   - Shared percentage/resource/latency helpers
========================================================================== */

var PRE_H_WARN = 80;
var PRE_H_WARN_CLR = 70;
var PRE_H_CRIT = 180;
var PRE_H_CRIT_CLR = 150;
var PRE_H_SAMPLES = 2;

function PREpercentClamp(Val) {
   var pct = parseInt(Val, 10);
   if (!Number.isFinite(pct)) pct = 0;
   if (pct < 0) pct = 0;
   if (pct > 100) pct = 100;
   return pct;
}

function PREresourceMemPercent(Res) {
   if (!Res) return 0;
   var direct = parseFloat(Res.mem_percent);
   if (Number.isFinite(direct)) return PREpercentClamp(direct);

   var resident = parseFloat(Res.mem_resident_mb);
   var physical = parseFloat(Res.mem_physical_mb);
   if (Number.isFinite(resident) && Number.isFinite(physical) && physical > 0) {
      return PREpercentClamp((resident / physical) * 100);
   }
   return 0;
}

function PRElatencyTierImmediate(IsConnected, LatMs, Config) {
   var cfg = Config || {};
   if (!IsConnected) return "inactive";
   var ms = parseFloat(LatMs);
   if (!Number.isFinite(ms)) return "ok";
   if (ms > Number(cfg.criticalMs || 180)) return "critical";
   if (ms > Number(cfg.warnMs || 80)) return "warn";
   return "ok";
}

function PRElatencyTierTarget(IsConnected, LatMs, StableTier, Config) {
   var cfg = Config || {};
   if (!IsConnected) return "inactive";
   var ms = parseFloat(LatMs);
   if (!Number.isFinite(ms)) return "ok";
   if (ms < 0) ms = 0;

   var warnMs = Number(cfg.warnMs || 80);
   var warnClearMs = Number(cfg.warnClearMs || 70);
   var criticalMs = Number(cfg.criticalMs || 180);
   var criticalClearMs = Number(cfg.criticalClearMs || 150);
   var tier = String(StableTier || "ok");

   if (tier === "critical") {
      if (ms < criticalClearMs) {
         if (ms > warnMs) return "warn";
         return "ok";
      }
      return "critical";
   }

   if (tier === "warn") {
      if (ms > criticalMs) return "critical";
      if (ms < warnClearMs) return "ok";
      return "warn";
   }

   if (ms > criticalMs) return "critical";
   if (ms > warnMs) return "warn";
   return "ok";
}

function PRElatencyTierWithHysteresis(StateMap, Key, IsConnected, LatMs, Config) {
   var cfg = Config || {};
   if (!StateMap) return PRElatencyTierImmediate(IsConnected, LatMs, cfg);

   var key = String(Key || "");
   if (!key) return PRElatencyTierImmediate(IsConnected, LatMs, cfg);

   if (!IsConnected) {
      delete StateMap[key];
      return "inactive";
   }

   var state = StateMap[key];
   if (!state) {
      var initialTier = PRElatencyTierImmediate(IsConnected, LatMs, cfg);
      StateMap[key] = { tier: initialTier, candidate: "", samples: 0 };
      return initialTier;
   }

   var targetTier = PRElatencyTierTarget(IsConnected, LatMs, state.tier, cfg);
   if (targetTier === state.tier) {
      state.candidate = "";
      state.samples = 0;
      return state.tier;
   }

   if (state.candidate !== targetTier) {
      state.candidate = targetTier;
      state.samples = 1;
      return state.tier;
   }

   state.samples += 1;
   var confirmSamples = Number(cfg.confirmSamples || 2);
   if (state.samples >= confirmSamples) {
      state.tier = targetTier;
      state.candidate = "";
      state.samples = 0;
   }
   return state.tier;
}

function PRElatencyDotClassForTier(Tier, Classes) {
   var classes = Classes || {};
   var tier = String(Tier || "");
   if (tier === "critical") return String(classes.critical || "critical");
   if (tier === "warn") return String(classes.warn || "warn");
   if (tier === "inactive") return String(classes.inactive || "inactive");
   return String(classes.ok || "ok");
}

function PRElatencyTextClassForTier(Tier, Classes) {
   var classes = Classes || {};
   var tier = String(Tier || "");
   if (tier === "critical") return String(classes.critical || "");
   if (tier === "warn") return String(classes.warn || "");
   return String(classes.ok || "");
}

function PREpeerGridLatencyTierImmediate(IsConnected, LatMs) {
   return PRElatencyTierImmediate(IsConnected, LatMs, {
      warnMs: PRE_H_WARN,
      criticalMs: PRE_H_CRIT
   });
}

function PREpeerGridLatencyTierWithHysteresis(StateMap, Key, IsConnected, LatMs) {
   return PRElatencyTierWithHysteresis(StateMap, Key, IsConnected, LatMs, {
      warnMs: PRE_H_WARN,
      warnClearMs: PRE_H_WARN_CLR,
      criticalMs: PRE_H_CRIT,
      criticalClearMs: PRE_H_CRIT_CLR,
      confirmSamples: PRE_H_SAMPLES
   });
}

function PREpeerGridLatencyDotClassForTier(Tier) {
   return PRElatencyDotClassForTier(Tier, {
      critical: "PREdotCritical",
      warn: "PREdotWarn",
      inactive: "PREdotInactive",
      ok: "PREdotActive"
   });
}

function PREpeerGridLatencyTextClassForTier(Tier) {
   return PRElatencyTextClassForTier(Tier, {
      critical: "PRElatencyCritical",
      warn: "PRElatencyWarn",
      ok: ""
   });
}
