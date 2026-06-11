function LGEargs(config = {}) {
   const {
     guid = "",
     error = false,
     limit = 3
   } = config;

   let component = guid || PAGEhashParameter("component");

   let Args = {
     component,
     limit: limit,
     exact: true,
     formatted: true,
     relvant: true,
   };

   if (error) {
     Args.tags = "#error";
     Args.critical = false;
   }

   return Args;
 }
