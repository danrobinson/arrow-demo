#include "builder.h"
#include "take_alt.h"
#include "map.h"

namespace arrow {

template<typename index_type, typename array_type, typename return_type = MapReturnType<GetOperation, index_type, array_type> >
return_type Take(array_type& array, index_type& index_array) {
  return map<GetOperation>(index_array, array);
}

} // namespace arrow
