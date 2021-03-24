#include "environment.h"
#include "instance.h"

namespace laplace::vm {
  instance::instance() { }
  instance::~instance() { }

  void instance::new_thread(size_t adress, cref_vbyte args) {
    if (adress < m_bitcode.size()) {
      execute::state s;
      s.is_active           = true;
      s.instruction_pointer = adress;
      s.stack.assign(args.begin(), args.end());

      m_threads.emplace_back(s);
    }
  }

  auto instance::perform(size_t id, cref_vbyte args) -> vbyte {
    vbyte result;
    auto  i = m_exports.find(id);

    if (i != m_exports.end()) {
      execute::state s;
      s.is_active           = true;
      s.instruction_pointer = i->second;
      s.fix_scale           = execute::default_fix_scale;
      s.stack.assign(args.begin(), args.end());

      execute(s);

      result = s.stack;
    }

    return result;
  }

  void instance::tick() {
    for (size_t i = 0; i < m_threads.size();) {
      execute(m_threads[i]);

      if (!m_threads[i].is_active) {
        m_threads.erase(m_threads.begin() + static_cast<ptrdiff_t>(i));
      } else {
        i++;
      }
    }
  }

  void instance::execute(execute::ref_state s) {
    auto fork = [this](size_t address, cref_vbyte args) {
      new_thread(address, args);
    };

    auto call_ext = [this](size_t id, cref_vbyte args) -> vbyte {
      if (auto e = get_env(); e) {
        return e->perform(id, args);
      }

      return {};
    };

    auto exp = [this](size_t id, size_t address) {
      if (auto e = get_env(); e) {
        m_exports.emplace(std::pair { id, address });
        e->add(id, shared_from_this());
      }
    };

    execute::tick(s, m_bitcode, m_heap, fork, call_ext, exp,
                  execute::default_instructions_limit);
  }
}
