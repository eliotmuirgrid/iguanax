function DOC2convertDate(Created) {
   let TimeStamp = parseInt(Created, 16);
   const FullDate = new Date(TimeStamp * 1000);
 
   const days = ["Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"];
   const months = [
     "January", "February", "March", "April", "May", "June",
     "July", "August", "September", "October", "November", "December"
   ];
 
   function getOrdinal(n) {
     if (n > 3 && n < 21) return n + "th";
     switch (n % 10) {
       case 1: return n + "st";
       case 2: return n + "nd";
       case 3: return n + "rd";
       default: return n + "th";
     }
   }
 
   const dayName = days[FullDate.getDay()];
   const day = getOrdinal(FullDate.getDate());
   const month = months[FullDate.getMonth()];
   const year = FullDate.getFullYear();
 
   return `${dayName} ${day} ${month} ${year}`;
 }