import { rpc } from './rpc'
import md5 from 'js-md5'

export const session = {
  aclCache: null
}

session.sid = function () {
  return sessionStorage.getItem('sid') || ''
}

session.login = function (password) {
  return new Promise(resolve => {
    if (typeof (password) !== 'string') {
      password = ''
    }

    if (password) password = md5(password)

    rpc.login(password).then(r => {
      sessionStorage.setItem('sid', r.sid)
      this.startHeartbeat()
      resolve(true)
    }).catch(() => {
      resolve(false)
    })
  })
}

session.logout = function () {
  return new Promise(resolve => {
    const sid = this.sid()
    if (!sid) {
      resolve()
      return
    }

    rpc.logout().then(() => {
      resolve()
    }).catch(() => {
      resolve()
    })

    this.stopHeartbeat()
    sessionStorage.removeItem('sid')
  })
}

session.isAlive = function () {
  return new Promise(resolve => {
    rpc.alive().then(() => {
      resolve(true)
    }).catch(() => {
      resolve(false)
    })
  })
}

session.startHeartbeat = function () {
  if (typeof (this._hearbeatInterval) !== 'undefined') {
    return
  }

  this._hearbeatInterval = window.setInterval(() => {
    this.isAlive().then(alive => {
      if (!alive) {
        this.logout()
      }
    })
  }, 15000)
}

session.stopHeartbeat = function () {
  if (typeof (this._hearbeatInterval) !== 'undefined') {
    window.clearInterval(this._hearbeatInterval)
    delete this._hearbeatInterval
  }
}

export default {
  install (Vue) {
    Vue.prototype.$session = session
  }
}
