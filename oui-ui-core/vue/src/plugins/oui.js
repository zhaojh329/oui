(function () {
  if (typeof (window.oui) !== 'undefined') { return }
  window.oui = {
    isEmptyArray (v) {
      return Array.isArray(v) && v.length === 0
    },

    isEqual (obj1, obj2) {
      if (typeof (obj1) === 'undefined' || typeof (obj2) === 'undefined') { return obj1 === obj2 }

      const v1 = obj1.valueOf()
      const v2 = obj2.valueOf()

      if (typeof (v1) !== typeof (v2)) { return false }

      if (typeof (v1) === 'object') {
        if (Array.isArray(v1)) {
          if (v1.length !== v2.length) { return false }
          for (let i = 0; i < v1.length; i++) {
            if (!window.oui.isEqual(v1[i], v2[i])) { return false }
          }
          return true
        } else {
          if (Object.keys(v1).length !== Object.keys(v2).length) { return false }
          for (const k in v1) {
            if (!window.oui.isEqual(v1[k], v2[k])) { return false }
          }
          return true
        }
      } else {
        return v1 === v2
      }
    },

    isPromise (obj) {
      return typeof (obj) === 'object' && obj !== null &&
        typeof (obj.then) === 'function' && typeof (obj.catch) === 'function'
    }
  }
})()
