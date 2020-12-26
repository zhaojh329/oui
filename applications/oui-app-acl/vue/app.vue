<template>
  <div>
    <span>{{ $t('acl.group')  }}</span>
    <a-select style="margin-left: 5px; margin-right: 10px; margin-bottom: 10px; width: 120px" v-model="group" @select="onGroupSelect">
      <a-select-option v-for="group in groups" :value="group" :key="group">{{ group }}</a-select-option>
    </a-select>
    <a-button type="primary" @click="addGroup">{{ $t('acl.addgroup') }}</a-button>
    <a-button type="danger" style="margin-left: 5px;" @click="delGroup" :disabled="!group">{{ $t('acl.delgroup') }}</a-button>
    <a-button type="primary" style="margin-left:30px;" @click="dialogVisible = true" :disabled="!group">{{ $t('acl.addacl') }}</a-button>
    <a-table :columns="columns" :data-source="acls" :loading="loading">
      <template #del="text">
        <a-button type="danger" size="small" @click="handleDelete(text)">{{ $t('Delete') }}</a-button>
      </template>
    </a-table>
    <a-modal v-model="dialogVisible" title="Add" @cancel="handleCancel" @ok="handleAdd">
      <a-form-model :model="form" ref="form" :rules="rules" :label-col="labelCol" :wrapper-col="wrapperCol">
        <a-form-model-item :label="$t('acl.scope')" prop="scope">
          <a-input v-model="form.scope"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('acl.entry')" prop="entry">
          <a-input v-model="form.entry"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('acl.perm')" prop="perm">
          <a-input v-model="form.perm"/>
        </a-form-model-item>
      </a-form-model>
    </a-modal>
  </div>
</template>

<script>
export default {
  data () {
    return {
      loading: true,
      columns: [
        { dataIndex: 'scope', title: this.$t('acl.scope') },
        { dataIndex: 'entry', title: this.$t('acl.entry') },
        { dataIndex: 'perm', title: this.$t('acl.perm') },
        { key: 'del', scopedSlots: { customRender: 'del' } }
      ],
      group: '',
      newGroup: '',
      groups: [],
      acls: [],
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      dialogVisible: false,
      form: {
        scope: '',
        entry: '',
        perm: ''
      },
      rules: {
        scope: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        entry: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        perm: [
          { required: true, message: () => this.$t('validator.required') }
        ]
      }
    }
  },
  methods: {
    fetch (group) {
      this.$rpc.call('acl', 'list', { group }).then(({ acls }) => {
        this.acls = acls.map((v, i) => {
          v.key = i
          return v
        })
        this.loading = false
      })
    },
    onGroupSelect () {
      this.fetch(this.group)
    },
    addGroup () {
      this.$prompt({
        title: this.$t('acl.addgroup')
      }).then(val => {
        this.$rpc.call('acl', 'add_group', { name: val }).then(() => {
          this.groups.push(val)
          this.group = val
          this.fetch(this.group)
        })
      })
    },
    delGroup () {
      this.$rpc.call('acl', 'del_group', { name: this.group }).then(() => {
        this.groups.splice(this.groups.indexOf(this.group), 1)

        if (this.groups.length > 0) {
          this.group = this.groups[0]
          this.fetch(this.group)
        } else {
          this.group = ''
          this.acls = []
        }
      })
    },
    handleCancel () {
      this.$refs.form.resetFields()
    },
    handleAdd () {
      this.$refs.form.validate(valid => {
        if (!valid) { return }

        this.dialogVisible = false
        this.loading = true

        this.$rpc.call('acl', 'add', { group: this.group, ...this.form }).then(() => {
          this.fetch(this.group)
        })

        this.$refs.form.resetFields()
      })
    },
    handleDelete (acl) {
      this.$rpc.call('acl', 'del', { group: this.group, ...acl }).then(() => {
        this.fetch(this.group)
      })
    }
  },
  created () {
    this.$rpc.call('acl', 'groups').then(({ groups }) => {
      this.groups = groups
      if (groups.length > 0) {
        this.group = groups[0]
        this.fetch(this.group)
      } else {
        this.loading = false
      }
    })
  }
}
</script>
