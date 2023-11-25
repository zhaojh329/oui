<template>
  <el-button type="primary" @click="showAdd">{{ $t('Add user') }}</el-button>
  <el-dialog v-model="model" :title="modify ? $t('Change') : $t('Add user')">
    <el-form ref="form" :model="formValue" :rules="rules" label-width="auto" label-suffix=":" size="large">
      <el-form-item :label="$t('Username')" prop="username">
        <el-input v-model="formValue.username" :readonly="modify !== ''"/>
      </el-form-item>
      <el-form-item :label="$t('Password')" prop="password">
        <el-input v-model="formValue.password" type="password" show-password/>
      </el-form-item>
      <el-form-item :label="$t('ACL group')" prop="acl">
        <el-select v-model="formValue.acl">
          <el-option v-for="o in aclGroups" :key="o" :label="o" :value="o"/>
        </el-select>
      </el-form-item>
    </el-form>
    <template #footer>
      <el-button @click="model = false">{{ $t('Cancel') }}</el-button>
      <el-button type="primary" @click="addUser">{{ $t('OK') }}</el-button>
    </template>
  </el-dialog>
  <el-divider/>
  <el-table :data="users">
    <el-table-column prop="username" :label="$t('Username')" width="200"/>
    <el-table-column>
      <template #default="{ row }">
        <el-space>
          <el-button type="primary" @click="modifyUser(row)">{{ $t('Change') }}</el-button>
          <el-button type="danger" @click="deleteUser(row)" :disabled="row.username === 'admin'">{{ $t('Delete') }}</el-button>
        </el-space>
      </template>
    </el-table-column>
  </el-table>
</template>

<script>
import { nextTick } from 'vue'

export default {
  data() {
    return {
      model: false,
      modify: '',
      users: [],
      rules: {
        username: {
          required: true,
          trigger: 'blur',
          message: () => this.$t('This field is required')
        },
        password: {
          required: true,
          trigger: 'blur',
          message: () => this.$t('This field is required')
        },
        acl: {
          required: true,
          trigger: 'blur',
          message: () => this.$t('This field is required')
        }
      },
      formValue: {
        username: '',
        password: '',
        acl: ''
      },
      aclGroups: []
    }
  },
  methods: {
    getUsers() {
      this.$oui.call('user', 'get_users').then(({ users }) => this.users = users)
    },
    deleteUser(user) {
      this.$confirm(this.$t('delete-user-confirm', { username: user.username }), this.$t('Delete'), {
        type: 'warning'
      }).then(() => this.$oui.call('user', 'del_user', { id: user.id }).then(() => this.getUsers()))
    },
    showAdd() {
      this.modify = ''
      this.formValue.username = ''
      this.formValue.password = ''
      this.formValue.acl = this.aclGroups[0]
      this.model = true
      nextTick(() => this.$refs.form.clearValidate())
    },
    addUser() {
      this.$refs.form.validate(valid => {
        if (!valid)
          return

        if (this.modify) {
          this.$oui.call('user', 'change', {
            password: this.formValue.password,
            acl: this.formValue.acl,
            id: this.modify
          })
          this.model = false
        } else {
          this.$oui.call('user', 'add_user', {
            username: this.formValue.username,
            password: this.formValue.password,
            acl: this.formValue.acl
          }).then(({ code }) => {
            if (code === 0) {
              this.model = false
              this.getUsers()
            } else {
              this.$message.error(this.$t('username-exist', { username: this.formValue.username }))
            }
          })
        }
      })
    },
    modifyUser(r) {
      this.modify = r.id
      this.formValue.username = r.username
      this.formValue.password = ''
      this.formValue.acl = r.acl
      this.model = true
      nextTick(() => this.$refs.form.clearValidate())
    }
  },
  created() {
    this.getUsers()
    this.$oui.call('acl', 'load').then(acls => this.aclGroups = Object.keys(acls))
  }
}
</script>

<i18n src="./locale.json"/>
