<template>
  <div>
    <a-button type="primary" style="margin-bottom: 10px" @click="showAdd">{{ $t('Add') }}</a-button>
    <a-table :columns="columns" :data-source="dataSource">
      <template #edit="text">
        <a-button type="primary" size="small" @click="showEdit(text)" style="margin-right: 10px">{{ $t('Edit') }}</a-button>
        <a-button :disabled="text.username === 'admin'" type="danger" size="small" @click="handleDelete(text.username)">{{ $t('Delete') }}</a-button>
      </template>
    </a-table>
    <a-modal v-model="dialogVisible" :title="add ? $t('Add') : $t('Edit')" @cancel="handleCancel" @ok="handleEdit">
      <a-form-model :model="edit" ref="edit" :rules="rules" :label-col="labelCol" :wrapper-col="wrapperCol">
        <a-form-model-item :label="$t('users.username')" prop="username">
          <a-input v-model="edit.username" :read-only="!add"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('users.aclgroup')" prop="acl">
          <a-input v-model="edit.acl"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('users.password')" prop="pw1">
          <a-input-password v-model="edit.pw1"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('users.pwconfirm')" prop="pw2">
          <a-input-password v-model="edit.pw2"/>
        </a-form-model-item>
      </a-form-model>
    </a-modal>
  </div>
</template>

<script>
export default {
  data () {
    const validatePass = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('users.Please enter your password')))
      } else {
        if (this.edit.pw2 !== '') { this.$refs.edit.validateField('pw2') }
        callback()
      }
    }

    const validatorConfirm = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('users.Please enter your password again')))
      } else if (value !== this.edit.pw1) {
        callback(new Error(this.$t('users.Inconsistent input password twice!')))
      } else {
        callback()
      }
    }

    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      columns: [
        { dataIndex: 'username', title: this.$t('users.username'), width: 140 },
        { dataIndex: 'acl', title: this.$t('users.aclgroup'), width: 140 },
        { key: 'edit', width: 140, scopedSlots: { customRender: 'edit' } }
      ],
      users: [],
      dialogVisible: false,
      add: false,
      edit: {
        username: '',
        acl: '',
        pw1: '',
        pw2: ''
      },
      rules: {
        username: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        acl: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        pw1: [
          { validator: validatePass }
        ],
        pw2: [
          { validator: validatorConfirm }
        ]
      }
    }
  },
  computed: {
    dataSource () {
      return this.users.map((v, i) => {
        v.key = i
        return v
      })
    }
  },
  methods: {
    getUsers () {
      return new Promise(resolve => {
        this.$rpc.call('users', 'get').then(({ users }) => {
          this.users = users
          resolve()
        })
      })
    },
    showEdit (user) {
      Object.assign(this.edit, user)
      this.dialogVisible = true
      this.add = false
    },
    showAdd () {
      this.dialogVisible = true
      this.add = true
    },
    handleCancel () {
      this.$refs.edit.resetFields()
    },
    handleDelete (username) {
      const content = this.$t('users.DelConfirm', { username })
      this.$confirm({
        title: this.$t('Delete'),
        content,
        onOk: () => {
          this.$rpc.call('users', 'del', { username }).then(() => {
            this.getUsers()
            this.$message.success(this.$t('users.success'))
          })
        }
      })
    },
    handleEdit () {
      this.$refs.edit.validate(valid => {
        if (!valid) { return }

        this.$rpc.call('users', this.add ? 'add' : 'modify', {
          username: this.edit.username,
          password: this.edit.pw1,
          acl: this.edit.acl
        }).then(() => {
          this.getUsers()
          this.$message.success(this.$t('users.success'))
        })

        this.dialogVisible = false
      })
    }
  },
  created () {
    this.getUsers()
  }
}
</script>
