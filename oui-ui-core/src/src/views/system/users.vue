<template>
  <uci-form config="rpcd" @applied="onApplied">
    <uci-section type="login" addable :teasers="['username', 'shadow']" :add="addUser">
      <uci-option-dummy :label="$t('Username')" name="username"></uci-option-dummy>
      <uci-option-switch :label="$t('Use the Linux system user password')" name="shadow" :load="isShadow" @change="shadowChanged" :save="saveShadow"></uci-option-switch>
      <uci-option-input :label="$t('Password')" name="password" depend="!shadow" required password :description="$t('acl-password-description')" :save="savePasswd"></uci-option-input>
      <uci-option :label="$t('User ACLs')" name="acls" :load="loadAcls" :save="saveAcls" :description="$t('acl-acl-description')">
        <template v-slot="{value}">
          <el-table :data="value || []" class="oui-acls-table">
            <el-table-column :label="$t('ACL Group')" prop="description"></el-table-column>
            <el-table-column label="N" width="30">
              <template v-slot="{row}">
                <el-radio v-model="row.acl" label="n"></el-radio>
              </template>
            </el-table-column>
            <el-table-column label="R" width="30">
              <template v-slot="{row}">
                <el-radio v-model="row.acl" label="r"></el-radio>
              </template>
            </el-table-column>
            <el-table-column label="F" width="30">
              <template v-slot="{row}">
                <el-radio v-model="row.acl" label="f"></el-radio>
              </template>
            </el-table-column>
          </el-table>
        </template>
      </uci-option>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      groups: null
    }
  },
  methods: {
    cryptPassword(data) {
      return this.$ubus.call('oui.ui', 'crypt', {data});
    },
    callACLs() {
      return this.$ubus.call('oui.ui', 'acls');
    },
    isShadow(sid) {
      let r = false;
      const pw = this.$uci.get('rpcd', sid, 'password');
      if (pw && pw.indexOf('$p$') === 0)
        r = true;
      return r;
    },
    shadowChanged(v, sid, self) {
      const s = self.uciSection;
      const pw = s.formValue('password', sid);

      if (!v && pw && pw.indexOf('$p$') === 0)
        s.setFormValue('password', sid, '');
    },
    doSavePasswd(s, sid) {
      const username = s.formValue('username', sid);
      const sh = s.formValue('shadow', sid);
      let pw = s.formValue('password', sid);

      if (sh)
        pw = '$p$' + username;

      if (pw.match(/^\$[0-9p][a-z]?\$/)) {
        if (pw !== this.$uci.get('rpcd', sid, 'password'))
          this.$uci.set('rpcd', sid, 'password', pw);
      } else {
        return new Promise(resolve => {
          this.cryptPassword(pw).then(({crypt}) => {
            if (crypt !== this.$uci.get('rpcd', sid, 'password'))
              this.$uci.set('rpcd', sid, 'password', crypt);
            resolve();
          });
        });
      }
    },
    saveShadow(sid, value, self) {
      return this.doSavePasswd(self.uciSection, sid);
    },
    savePasswd(sid, value, self) {
      return this.doSavePasswd(self.uciSection, sid);
    },
    addUser(self) {
      return new Promise(resolve => {
        this.$prompt(this.$t('Please input a username'), this.$t('Add'), {
          inputValidator: value => {
            if (!value)
              return true;

            const sections = self.sections;
            for (let i = 0; i < sections.length; i++)
              if (sections[i].username === value)
                return this.$t('Username already used');

            return true;
          }
        }).then(r => {
          if (!r.value)
            return;

          const sid = this.$uci.add('rpcd', 'login');
          this.$uci.set('rpcd', sid, 'username', r.value);
          resolve(sid);
        });
      });
    },
    mergeACLScope(aclScope, scope) {
      if (Array.isArray(scope)) {
        scope.forEach(name => {
          aclScope[name] = true;
        });
      } else {
        for (const name in scope) {
          if (!Array.isArray(scope[name]))
            continue;
          const aclObject = aclScope[name] || (aclScope[name] = {});
          scope[name].forEach(s => {
            aclObject[s] = true;
          });
        }
      }
    },
    mergeACLPermission(aclPerm, perm) {
      for (const name in perm) {
        const aclScope = aclPerm[name] || (aclPerm[name] = {});
        this.mergeACLScope(aclScope, perm[name]);
      }
    },
    mergeACLGroup(aclAroup, group) {
      if (!aclAroup.description)
        aclAroup.description = group.description;

      if (group.read) {
        const aclPerm = aclAroup.read || (aclAroup.read = {});
        this.mergeACLPermission(aclPerm, group.read);
      }

      if (group.write) {
        const aclPerm = aclAroup.write || (aclAroup.write = {});
        this.mergeACLPermission(aclPerm, group.write);
      }
    },
    aclMatch(list, group) {
      if (list.indexOf('!' + group) > -1)
        return false;

      if (list.indexOf(group) > -1)
        return true;

      if (list[0] === '*')
        return true;
    },
    loadGroups() {
      return new Promise(resolve => {
        if (this.groups) {
          resolve();
          return;
        }

        this.callACLs().then(r => {
          const acls = {};

          r.acls.forEach(tree => {
            for (const name in tree) {
              const aclAroup = acls[name] || (acls[name] = {});
              this.mergeACLGroup(aclAroup, tree[name]);
            }
          });

          const groups = [];

          for (const groupName in acls) {
            if (groupName === 'unauthenticated')
              continue;
            groups.push({
              description: this.$t(acls[groupName].description),
              name: groupName
            });
          }

          this.groups = groups;
          resolve();
        });
      });
    },
    loadAcls(sid) {
      return new Promise(resolve => {
        this.loadGroups().then(() => {
          const readList = this.$uci.get('rpcd', sid, 'read') || [];
          const writeList = this.$uci.get('rpcd', sid, 'write') || [];
          const groups = [];

          this.groups.forEach(g => {
            const r = this.aclMatch(readList, g.name);
            const w = this.aclMatch(writeList, g.name);

            groups.push({
              acl: w ? 'f' : (r ? 'r' : 'n'),
              ...g
            });
          });

          resolve(groups);
        });
      });
    },
    aclToUCI(list) {
      if (list.length < this.groups.length / 2)
        return list;

      const set = {};
      list.forEach(v => {
        set[v] = true;
      });

      const rv = ['*'];

      this.groups.forEach(g => {
        if (!set[g.name])
          rv.push('!' + g.name);
      });

      return rv;
    },
    saveAcls(sid, value) {
      let readList = [];
      let writeList = [];

      value.forEach(group => {
        const name = group.name;
        const acl = group.acl;
        if (acl === 'f') {
          readList.push(name);
          writeList.push(name);
        } else if (acl === 'r') {
          readList.push(name);
        }
      });

      readList = this.aclToUCI(readList);
      writeList = this.aclToUCI(writeList);

      const originalReadList = this.$uci.get('rpcd', sid, 'read') || [];
      const originalWriteList = this.$uci.get('rpcd', sid, 'write') || [];

      if (!window.oui.isEqual(readList, originalReadList))
        this.$uci.set('rpcd', sid, 'read', this.aclToUCI(readList));

      if (!window.oui.isEqual(writeList, originalWriteList))
        this.$uci.set('rpcd', sid, 'write', this.aclToUCI(writeList));
    },
    onApplied() {
      this.$session.updateACLs();
    }
  }
}
</script>

<style lang="scss">
.oui-acls-table {
  .el-radio__label {
    display: none;
  }
}
</style>
