#ifndef _DB_TRANSACTION_SETTINGS_H
#define _DB_TRANSACTION_SETTINGS_H

namespace tx {
enum class Commit : bool { BLOCKING = 0, NON_BLOCKING = 1 };
struct Settings {
public:
  const Commit m_commit;

  Settings() : m_commit(Commit::BLOCKING) {
  }

private:
};
}

#endif
