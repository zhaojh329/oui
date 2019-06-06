<template>
  <div>
    <el-button type="primary" size="mini" style="margin-bottom: 10px" @click="showAdd">Add</el-button>
    <el-table :data="data">
      <el-table-column prop="min" label="Minute" width="140"></el-table-column>
      <el-table-column prop="hour" label="Hour" width="140"></el-table-column>
      <el-table-column prop="day" label="Day" width="140"></el-table-column>
      <el-table-column prop="month" label="Month" width="140"></el-table-column>
      <el-table-column prop="week" label="Week" width="140"></el-table-column>
      <el-table-column prop="command" label="Command"></el-table-column>
      <el-table-column label="#" width="200">
        <template v-slot="{$index}">
          <el-button type="primary" size="mini" @click="showEdit($index)">Edit</el-button>
          <el-button type="danger" size="mini" @click="handleDelete($index)">Delete</el-button>
        </template>
      </el-table-column>
    </el-table>
    <el-dialog :visible.sync="dialogVisible" :title="add ? 'Add' : 'Edit'">
      <el-form label-width="100px" label-position="left" :model="edit" ref="edit">
        <el-form-item label="Minute" prop="min" required>
          <el-input v-model="edit.min"></el-input>
        </el-form-item>
        <el-form-item label="Hour" prop="hour" required>
          <el-input v-model="edit.hour"></el-input>
        </el-form-item>
        <el-form-item label="Day" prop="day" required>
          <el-input v-model="edit.day"></el-input>
        </el-form-item>
        <el-form-item label="Month" prop="month" required>
          <el-input v-model="edit.month"></el-input>
        </el-form-item>
        <el-form-item label="Week" prop="week" required>
          <el-input v-model="edit.week"></el-input>
        </el-form-item>
        <el-form-item label="Command" prop="command" required>
          <el-input v-model="edit.command"></el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button @click="dialogVisible = false">Cancel</el-button>
        <el-button type="primary" @click="handleEdit">OK</el-button>
      </div>
    </el-dialog>
  </div>
</template>

<script>
export default {
  data() {
    return {
      data: [],
      datax: [
        {index: 0, min: '*', hour: '*', day: '*', month: '*', week: '*', command: 'reboot', editcmd: false},
        {index: 1, min: '*', hour: '*', day: '*', month: '*', week: '1', command: 'reboot fgh ghh hjj', editcmd: false}
      ],
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
      }
    }
  },
  computed: {
  },
  methods: {
    getCrontab() {
      return new Promise(resolve => {
        this.$ubus.call('oui.system', 'crontab_get').then(r => {
          this.data = [];
          const data = r.data.split('\n');
          let index = 0;
          data.forEach(item => {
            const cron = item.split(' ');
            if (cron.length < 6)
              return;
            this.data.push({
              index: index++,
              min: cron[0],
              hour: cron[1],
              day: cron[2],
              month: cron[3],
              week: cron[4],
              command: cron[5]
            });
          });
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
      const content = `Are you sure you want to delete "${cron.min} ${cron.hour} ${cron.day} ${cron.month} ${cron.week} ${cron.command}"?`;
      this.$confirm(content, 'Delete').then(() => {
        this.data.splice(index, 1);
        this.apply().then(() => {
          this.$message.success('Delete ok');
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
          this.$message.success(this.add ? ' Add OK ' : 'Edit ok');
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
  mounted() {
    this.getCrontab();
  }
}
</script>
