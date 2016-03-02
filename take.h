#include "builder.h"
#include "apply.h" // foreach, get_from, append_to, and skip
#include "take_alt.h"
#include "timer.h"
#include "skip.h"

namespace arrow {


// trick for getting the type of the array returned by Take
// return type is nullable iff one of the arrays given to it is nullable
template<typename index_type, typename array_type>
using TakeReturnType = MapReturnType<GetOperation, index_type, Array<array_type> >;

// Take function called by user
template<typename index_type, typename array_type, typename return_type = TakeReturnType<index_type, array_type> >
const Array<return_type>* Take(const Array<index_type>& index_array, const Array<array_type>& array) {
  return map<GetOperation>(index_array, array);
}

} // namespace arrow
