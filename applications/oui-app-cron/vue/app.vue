<template>
  <div>
    <a-button type="primary" style="margin-bottom: 10px" @click="showAdd">{{ $t('Add') }}</a-button>
    <a-table :columns="columns" :data-source="dataSource">
      <template #edit="text, record, index">
        <a-button type="primary" size="small" @click="showEdit(index)" style="margin-right: 10px">{{ $t('Edit') }}</a-button>
        <a-button type="danger" size="small" @click="handleDelete(index)">{{ $t('Delete') }}</a-button>
      </template>
    </a-table>
    <a-modal v-model="dialogVisible" :title="add ? $t('Add') : $t('Edit')">
      <a-form-model :model="edit" ref="edit" :rules="rules" :label-col="labelCol" :wrapper-col="wrapperCol">
        <a-form-model-item :label="$t('cron.Minute')" prop="min">
          <a-input v-model="edit.min"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('cron.Hour')" prop="hour">
          <a-input v-model="edit.hour"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('cron.Day')" prop="day">
          <a-input v-model="edit.day"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('cron.Month')" prop="month">
          <a-input v-model="edit.month"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('cron.Week')" prop="week">
          <a-input v-model="edit.week"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('cron.Command')" prop="command">
          <a-input v-model="edit.command"/>
        </a-form-model-item>
      </a-form-model>
      <div slot="footer" class="dialog-footer">
        <a-button @click="dialogVisible = false">{{ $t('Cancel') }}</a-button>
        <a-button type="primary" @click="handleEdit">{{ $t('OK') }}</a-button>
      </div>
    </a-modal>
  </div>
</template>

<script>
export default {
  data () {
    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      columns: [
        { dataIndex: 'min', title: this.$t('cron.Minute'), width: 140 },
        { dataIndex: 'hour', title: this.$t('cron.Hour'), width: 140 },
        { dataIndex: 'day', title: this.$t('cron.Day'), width: 140 },
        { dataIndex: 'month', title: this.$t('cron.Month'), width: 140 },
        { dataIndex: 'week', title: this.$t('cron.Week'), width: 140 },
        { dataIndex: 'command', title: this.$t('cron.Command') },
        { key: 'edit', width: 140, scopedSlots: { customRender: 'edit' } }
      ],
      data: [],
      dialogVisible: false,
      add: false,
      editindex: 0,
      edit: {
        min: '',
        hour: '',
        day: '',
        month: '',
        week: '',
        command: ''
      },
      rules: {
        min: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        hour: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        day: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        month: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        week: [
          { required: true, message: () => this.$t('validator.required') }
        ],
        command: [
          { required: true, message: () => this.$t('validator.required') }
        ]
      }
    }
  },
  computed: {
    dataSource () {
      return this.data.map((v, i) => {
        v.key = i
        return v
      })
    }
  },
  methods: {
    getCrontab () {
      return new Promise(resolve => {
        this.$rpc.call('crontab', 'get').then(({ entries }) => {
          this.data = entries
          resolve()
        })
      })
    },
    setCrontab (data) {
      return this.$rpc.call('crontab', 'set', { data })
    },
    showEdit (index) {
      Object.assign(this.edit, this.data[index])
      this.editindex = index
      this.dialogVisible = true
      this.add = false
    },
    showAdd () {
      this.dialogVisible = true
      this.add = true
      this.edit.min = '*'
      this.edit.hour = '*'
      this.edit.day = '*'
      this.edit.month = '*'
      this.edit.week = '*'
      this.edit.command = ''
    },
    handleDelete (index) {
      const cron = this.data[index]
      const content = this.$t('cron.DelConfirm', { cron: `${cron.min} ${cron.hour} ${cron.day} ${cron.month} ${cron.week} ${cron.command}` })
      this.$confirm({
        title: this.$t('Delete'),
        content,
        onOk: () => {
          this.data.splice(index, 1)
          this.apply().then(() => {
            this.$message.success(this.$t('success'))
          })
        }
      })
    },
    handleEdit () {
      this.$refs.edit.validate(valid => {
        if (!valid) { return }

        if (this.add) {
          this.data.push(this.edit)
        } else {
          Object.assign(this.data[this.editindex], this.edit)
        }

        this.dialogVisible = false
        this.apply().then(() => {
          this.$message.success(this.$t('success'))
        })
      })
    },
    apply () {
      return new Promise(resolve => {
        const crons = []

        this.data.forEach(item => {
          const expr = `${item.min} ${item.hour} ${item.day} ${item.month} ${item.week} ${item.command}\n`
          crons.push(expr)
        })

        this.setCrontab(crons.join('')).then(() => {
          this.getCrontab().then(() => {
            resolve()
          })
        })
      })
    }
  },
  created () {
    this.getCrontab()
  }
}
</script>
