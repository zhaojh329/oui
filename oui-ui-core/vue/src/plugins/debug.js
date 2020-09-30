import { uci } from './uci'
import { rpc } from './rpc'

(function () {
  if (process.env.NODE_ENV === 'production') { return }

  const oui = window.oui

  if (typeof (oui.uci) === 'undefined') {
    oui.uci = {
      load: conf => {
        uci.load(conf).then(() => {
          if (typeof (console) !== 'undefined') { console.log('Load complete.') }
        })
      },
      sections: (conf, type) => {
        return uci.sections(conf, type)
      },
      get: (conf, sid, opt) => {
        return uci.get(conf, sid, opt)
      }
    }
  }

  if (typeof (oui.rpc) === 'undefined') {
    oui.rpc = function (object, method, params) {
      rpc.call(object, method, params).then(r => {
        if (typeof (console) !== 'undefined') { console.log(r) }
      })
    }
  }
})()
