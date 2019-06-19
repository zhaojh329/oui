<template>
  <uci-form config="rpcd">
    <uci-section type="login" addable :teasers="['username', 'shadow']">
      <uci-option type="input" :label="$t('Username')" name="username"></uci-option>
      <uci-option type="switch" :label="$t('Use the Linux system password')" name="shadow" :load="isShadow"></uci-option>
      <uci-option type="input" :label="$t('Password')" name="password" depend="!shadow" password></uci-option>
      <uci-option :label="$t('User ACLs')" name="acls" :description="$t('acls-description')" :load="loadAclsFromUCI">
        <template v-slot="prop">
          <el-table :data="groups" class="oui-acls-table">
            <el-table-column :label="$t('ACL Group')" prop="description"></el-table-column>
            <el-table-column label="N" width="30">
              <template v-slot="{row}">
                <el-radio v-model="row.value" label="n"></el-radio>
              </template>
            </el-table-column>
            <el-table-column label="R" width="30">
              <template v-slot="{row}">
                <el-radio v-model="row.value" label="r"></el-radio>
              </template>
            </el-table-column>
            <el-table-column label="F" width="30">
              <template v-slot="{row}">
                <el-radio v-model="row.value" label="w"></el-radio>
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
      acls: {},
      radio: '',
      groups: []
    }
  },
  methods: {
    callACLs() {
      return this.$ubus.call('oui.ui', 'acls');
    },
    isShadow(resolve, sid) {
      let r = '0';
      const pw = this.$uci.get('rpcd', sid, 'password');
      if (pw && pw.indexOf('$p$') === 0)
        r = '1';
      resolve(r);
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
    loadAclsFromUCI(resolve, sid) {
      const s = this.$uci.get('rpcd', sid);

      resolve({
        read: s.read || [],
        write: s.write || []
      });
    }
  },
  created() {
    this.callACLs().then(r => {
      const acls = {};

      r.acls.forEach(tree => {
        for (const name in tree) {
          const aclAroup = acls[name] || (acls[name] = {});
          this.mergeACLGroup(aclAroup, tree[name]);
        }
      });

      console.log(acls);

      this.acls = acls;

      for (const groupName in acls) {
        this.groups.push({
          description: acls[groupName].description
        });
      }
    });
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
