function CMTconvertedDate(DateString) {
   if (!DateString) return "";
   const now = Date.now();
   const commitDate = new Date(DateString);
   const timeDiffMs = now - commitDate.getTime();
   const timeDiffMinutes = Math.floor(timeDiffMs / 60000);
   const timeDiffHours = Math.floor(timeDiffMinutes / 60);
   const timeDiffDays = Math.floor(timeDiffHours / 24);
   if (timeDiffDays > 6) {
      return commitDate.toLocaleDateString();
   } else if (timeDiffHours >= 24) {
      return `${timeDiffDays} day${timeDiffDays > 1 ? "s" : ""} ago`;
   } else if (timeDiffMinutes >= 60) {
      return `${timeDiffHours} hour${timeDiffHours > 1 ? "s" : ""} ago`;
   } else {
      return `${timeDiffMinutes} minute${timeDiffMinutes > 1 ? "s" : ""} ago`;
   }
}

function CMTformatTime(DateString){
   if (!DateString) return "";
   let formattedTime = "";
   if(DateString) {
      let [dayOfWeek, month, day, time, year] = DateString.split(" ");
      formattedTime = time || "";
   }
   return  formattedTime;
}

function CMTformatDate(DateString){
   if (!DateString) return "";
   let formattedDate = "";
   if(DateString) {
      let [dayOfWeek, month, day, time, year] = DateString.split(" ");
      formattedDate = `${dayOfWeek || ""} ${month || ""} ${day || ""} ${year || ""}`.trim();
   }
   return formattedDate;
}
