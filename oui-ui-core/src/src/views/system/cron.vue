<template>
  <div>
    <el-button type="primary" size="mini" style="margin-bottom: 10px" @click="showAdd">{{ $t('Add') }}</el-button>
    <el-table :data="data">
      <el-table-column prop="min" :label="$t('Minute')" width="140"></el-table-column>
      <el-table-column prop="hour" :label="$t('Hour')" width="140"></el-table-column>
      <el-table-column prop="day" :label="$t('Day')" width="140"></el-table-column>
      <el-table-column prop="month" :label="$t('Month')" width="140"></el-table-column>
      <el-table-column prop="week" :label="$t('Week')" width="140"></el-table-column>
      <el-table-column prop="command" :label="$t('Command')"></el-table-column>
      <el-table-column label="#" width="200">
        <template v-slot="{$index}">
          <el-button type="primary" size="mini" @click="showEdit($index)">{{ $t('Edit') }}</el-button>
          <el-button type="danger" size="mini" @click="handleDelete($index)">{{ $t('Delete') }}</el-button>
        </template>
      </el-table-column>
    </el-table>
    <el-dialog :visible.sync="dialogVisible" :title="add ? $t('Add') : $t('Edit')">
      <el-form label-width="100px" label-position="left" :model="edit" ref="edit" :rules="rules">
        <el-form-item :label="$t('Minute')" prop="min">
          <el-input v-model="edit.min"></el-input>
        </el-form-item>
        <el-form-item :label="$t('Hour')" prop="hour">
          <el-input v-model="edit.hour"></el-input>
        </el-form-item>
        <el-form-item :label="$t('Day')" prop="day">
          <el-input v-model="edit.day"></el-input>
        </el-form-item>
        <el-form-item :label="$t('Month')" prop="month">
          <el-input v-model="edit.month"></el-input>
        </el-form-item>
        <el-form-item :label="$t('Week')" prop="week">
          <el-input v-model="edit.week"></el-input>
        </el-form-item>
        <el-form-item :label="$t('Command')" prop="command">
          <el-input v-model="edit.command"></el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button @click="dialogVisible = false">{{ $t('Cancel') }}</el-button>
        <el-button type="primary" @click="handleEdit">{{ $t('OK') }}</el-button>
      </div>
    </el-dialog>
  </div>
</template>

<script>
export default {
  data() {
    return {
      data: [],
      dialogVisible: false,
      add: false,
      edit: {
        index: 0,
        min: '',
        hour: '',
        day: '',
        month: '',
        week: '',
        command: ''
      },
      rules: {
        min: [
          {required: true, message: this.$t('This field is required')}
        ],
        hour: [
          {required: true, message: this.$t('This field is required')}
        ],
        day: [
          {required: true, message: this.$t('This field is required')}
        ],
        month: [
          {required: true, message: this.$t('This field is required')}
        ],
        week: [
          {required: true, message: this.$t('This field is required')}
        ],
        command: [
          {required: true, message: this.$t('This field is required')}
        ]
      }
    }
  },
  methods: {
    getCrontab() {
      return new Promise(resolve => {
        this.$ubus.call('oui.system', 'crontab_get').then(r => {
          let index = 0;
          const data = []

          r.entries.forEach(item => {
            data.push({
              index: index++,
              ...item
            });
          });
          this.data = data;
          resolve();
        });
      });
    },
    setCrontab(data) {
      return this.$ubus.call('oui.system', 'crontab_set', {data});
    },
    showEdit(index) {
      Object.assign(this.edit, this.data[index]);
      this.dialogVisible = true;
      this.add = false;
      this.dialogTitle = 'Edit';
    },
    showAdd() {
      this.dialogVisible = true;
      this.add = true;
      this.edit.min = '*';
      this.edit.hour = '*';
      this.edit.day = '*';
      this.edit.month = '*';
      this.edit.week = '*';
      this.edit.command = '';
      this.index = this.data.length;
    },
    handleDelete(index) {
      const cron = this.data[index];
      const content = this.$t('cron-del-confirm', {cron: `${cron.min} ${cron.hour} ${cron.day} ${cron.month} ${cron.week} ${cron.command}`});
      this.$confirm(content, this.$t('Delete')).then(() => {
        this.data.splice(index, 1);
        this.apply().then(() => {
          this.$message.success(this.$t('success'));
        });
      });
    },
    handleEdit() {
      this.$refs['edit'].validate(valid => {
        if (!valid)
          return;

        if (this.add)
          this.data.push(this.edit);
        else
          Object.assign(this.data[this.edit.index], this.edit);

        this.dialogVisible = false;
        this.apply().then(() => {
          this.$message.success(this.$t('success'));
        });
      });
    },
    apply() {
      return new Promise(resolve => {
        const crons = [];

        this.data.forEach(item => {
          const expr = `${item.min} ${item.hour} ${item.day} ${item.month} ${item.week} ${item.command}\n`;
          crons.push(expr);
        });

        this.setCrontab(crons.join('')).then(() => {
          this.getCrontab().then(() => {
            resolve();
          });
        });
      });
    }
  },
  created() {
    this.getCrontab();
  }
}
</script>
