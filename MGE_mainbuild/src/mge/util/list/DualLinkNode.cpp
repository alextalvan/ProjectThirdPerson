#include <mge/util/list/DualLinkNode.hpp>
#include <mge/core/Destroyable.hpp>
#include <mge/core/Light.hpp>


template class DualLinkNode<Destroyable>;
template class DualLinkNode<Light>;

