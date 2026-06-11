#ifndef __AVT_CLEAR_H__
#define __AVT_CLEAR_H__


#define AVTclearScreenToEnd "0J"
#define AVTclearScreenToStart "1J"
#define AVTclearScreen "2J"

#define AVTclearLineToEnd "0K"
#define AVTclearLineToStart "1K"
#define AVTclearLine "2K"

class COLostream;
#include <COL/COLstring.h>

class AVTclear{
public:
   AVTclear(COLstring ClearType) : m_ClearType(ClearType) {}

   COLstring Clear() const { return "\033[" + m_ClearType; }
private:
   COLstring m_ClearType;
};

COLostream& operator<<(COLostream& Stream, const AVTclear& Clear);

#endif // End of defensive include