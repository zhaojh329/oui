<template>
  <oui-form uci-config="rpcd" @applied="onApplied">
    <oui-typed-section type="login" addremove :teasers="['username', 'shadow']" :add="addUser" v-slot="{s}" :card="false" @change="changed">
      <oui-form-item-dummy :uci-section="s" :label="$t('Username')" name="username"/>
      <oui-form-item-switch :uci-section="s" :label="$t('Use the Linux system user password')" name="shadow" :load="loadShadow" :save="savePasswd"/>
      <oui-form-item-input :uci-section="s" :label="$t('Password')" password name="password" depend="!shadow" required :save="savePasswd" :help="$t('acl-password-description')"/>
      <oui-form-item :uci-section="s" :label="$t('User ACLs')" name="acls" :load="loadAcls" :save="saveAcls" :help="$t('acl-acl-description')" v-slot="{ self }">
        <a-table :data-source="aclsDataSource(self.model)" :columns="aclColumns">
          <template #acls="record">
            <a-radio-group v-model="record.acl ">
              <a-radio value="n"/>
              <a-radio value="r"/>
              <a-radio value="f"/>
            </a-radio-group>
          </template>
        </a-table>
      </oui-form-item>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  data () {
    return {
      groups: null,
      aclColumns: [
        { dataIndex: 'description', title: this.$t('ACL Group') },
        { key: 'acls', title: `${this.$t('N')} / ${this.$t('R')} / ${this.$t('F')}`, width: 140, scopedSlots: { customRender: 'acls' } }
      ]
    }
  },
  methods: {
    aclsDataSource (acls) {
      acls = acls || []
      return acls.map((a, i) => {
        a.key = i
        return a
      })
    },
    cryptPassword (data) {
      return this.$ubus.call('oui.ui', 'crypt', { data })
    },
    callACLs () {
      return this.$ubus.call('oui.ui', 'acls')
    },
    loadShadow (self) {
      let r = false
      const pw = this.$uci.get('rpcd', self.sid, 'password')
      if (pw && pw.indexOf('$p$') === 0) r = true
      return r
    },
    changed (sid, s) {
      const isShadow = s.get(sid, 'shadow')
      const pw = s.get(sid, 'password')
      if (!isShadow && pw && pw.indexOf('$p$') === 0) { s.set(sid, 'password', '') }
    },
    savePasswd (self) {
      const sid = self.sid
      const username = self.ouiSection.get(self.sid, 'username')
      const sh = self.ouiSection.get(self.sid, 'shadow')
      let pw = self.ouiSection.get(self.sid, 'password')

      if (sh) { pw = '$p$' + username }

      if (pw.match(/^\$[0-9p][a-z]?\$/)) {
        if (pw !== this.$uci.get('rpcd', sid, 'password')) { this.$uci.set('rpcd', sid, 'password', pw) }
      } else {
        return new Promise(resolve => {
          this.cryptPassword(pw).then(({ crypt }) => {
            if (crypt !== this.$uci.get('rpcd', self.sid, 'password')) { this.$uci.set('rpcd', sid, 'password', crypt) }
            resolve()
          })
        })
      }
    },
    addUser (self) {
      this.$prompt({
        title: this.$t('Add user'),
        placeholder: this.$t('Please input a username'),
        validator: value => {
          if (self.sections.filter(s => s.username === value).length > 0) {
            return this.$t('Username already used')
          }
        }
      }).then(value => {
        const sid = self.addSection()
        this.$uci.set('rpcd', sid, 'username', value)
      }).catch(() => {})
    },
    mergeACLScope (aclScope, scope) {
      if (Array.isArray(scope)) {
        scope.forEach(name => {
          aclScope[name] = true
        })
      } else {
        for (const name in scope) {
          if (!Array.isArray(scope[name])) { continue }
          const aclObject = aclScope[name] || (aclScope[name] = {})
          scope[name].forEach(s => {
            aclObject[s] = true
          })
        }
      }
    },
    mergeACLPermission (aclPerm, perm) {
      for (const name in perm) {
        const aclScope = aclPerm[name] || (aclPerm[name] = {})
        this.mergeACLScope(aclScope, perm[name])
      }
    },
    mergeACLGroup (aclAroup, group) {
      if (!aclAroup.description) { aclAroup.description = group.description }

      if (group.read) {
        const aclPerm = aclAroup.read || (aclAroup.read = {})
        this.mergeACLPermission(aclPerm, group.read)
      }

      if (group.write) {
        const aclPerm = aclAroup.write || (aclAroup.write = {})
        this.mergeACLPermission(aclPerm, group.write)
      }
    },
    aclMatch (list, group) {
      if (list.indexOf('!' + group) > -1) { return false }

      if (list.indexOf(group) > -1) { return true }

      if (list[0] === '*') { return true }
    },
    loadGroups () {
      return new Promise(resolve => {
        if (this.groups) {
          resolve()
          return
        }

        this.callACLs().then(r => {
          const acls = {}

          r.acls.forEach(tree => {
            for (const name in tree) {
              const aclAroup = acls[name] || (acls[name] = {})
              this.mergeACLGroup(aclAroup, tree[name])
            }
          })

          const groups = []

          for (const groupName in acls) {
            if (groupName === 'unauthenticated') { continue }
            groups.push({
              description: this.$t(acls[groupName].description),
              name: groupName
            })
          }

          this.groups = groups
          resolve()
        })
      })
    },
    loadAcls (sid) {
      return new Promise(resolve => {
        this.loadGroups().then(() => {
          const readList = this.$uci.get('rpcd', sid, 'read') || []
          const writeList = this.$uci.get('rpcd', sid, 'write') || []
          const groups = []

          this.groups.forEach(g => {
            const r = this.aclMatch(readList, g.name)
            const w = this.aclMatch(writeList, g.name)

            groups.push({
              acl: w ? 'f' : (r ? 'r' : 'n'),
              ...g
            })
          })

          resolve(groups)
        })
      })
    },
    aclToUCI (list) {
      if (list.length < this.groups.length / 2) { return list }

      const set = {}
      list.forEach(v => {
        set[v] = true
      })

      const rv = ['*']

      this.groups.forEach(g => {
        if (!set[g.name]) { rv.push('!' + g.name) }
      })

      return rv
    },
    saveAcls (sid, value) {
      let readList = []
      let writeList = []

      value.forEach(group => {
        const name = group.name
        const acl = group.acl
        if (acl === 'f') {
          readList.push(name)
          writeList.push(name)
        } else if (acl === 'r') {
          readList.push(name)
        }
      })

      readList = this.aclToUCI(readList)
      writeList = this.aclToUCI(writeList)

      const originalReadList = this.$uci.get('rpcd', sid, 'read') || []
      const originalWriteList = this.$uci.get('rpcd', sid, 'write') || []

      if (!window.oui.isEqual(readList, originalReadList)) { this.$uci.set('rpcd', sid, 'read', this.aclToUCI(readList)) }

      if (!window.oui.isEqual(writeList, originalWriteList)) { this.$uci.set('rpcd', sid, 'write', this.aclToUCI(writeList)) }
    },
    onApplied () {
      this.$session.updateACLs()
    }
  }
}
</script>
