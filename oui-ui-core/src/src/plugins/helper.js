'use strict'

export default {
  install(Vue) {
    Vue.prototype.$bus = new Vue();

    Vue.prototype.$getParent = function(name) {
      let parent = this.$parent;

      if (!name)
        return parent;

      while (parent.$options.name !== name) {
        parent = parent.$parent;
      }

      return parent;
    }
  }
}
