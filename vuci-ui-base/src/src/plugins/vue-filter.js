const VueFilter = {};

VueFilter.install = function(Vue) {
  Vue.filter('format', function (value, type) {
    if (type === 't') {
      let td = 0, th = 0, tm = 0, ts = value || 0;

      if (ts > 60) {
          tm = Math.floor(ts / 60);
          ts = (ts % 60);
      }

      if (tm > 60) {
          th = Math.floor(tm / 60);
          tm = (tm % 60);
      }

      if (th > 24) {
          td = Math.floor(th / 24);
          th = (th % 24);
      }

      return `${td}d ${th}h ${tm}m ${ts}s`;
    } else if (type === 'm') {
      let units = [ '', 'K', 'M', 'G', 'T', 'P', 'E' ];
      let mf = 1000, pr = 2;

      if (arguments.length > 2) {
        mf = arguments[2];

        if (arguments.length > 3)
          pr = arguments[3];
      }

      let i;
      for (i = 0; (i < units.length) && (value > mf); i++)
        value /= mf;

      return value.toFixed(pr) + ' ' + units[i];
    }
  });
};

export default VueFilter;
