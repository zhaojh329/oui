(function() {
  if (typeof(window.oui) !== 'undefined')
    return;
  window.oui = {
    isEmptyArray(v) {
      return Array.isArray(v) && v.length === 0;
    }
  };
})()