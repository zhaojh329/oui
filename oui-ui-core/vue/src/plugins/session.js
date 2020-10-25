import { rpc } from './rpc'

export const session = {
  username: ''
}

session.sid = function () {
  return sessionStorage.getItem('sid') || ''
}

session.login = function (username, password) {
  return new Promise(resolve => {
    if (typeof (password) !== 'string') {
      password = ''
    }

    rpc.login(username, password).then(r => {
      sessionStorage.setItem('sid', r.sid)
      this.username = username
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
    this.username = ''
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
