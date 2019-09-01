<template>
  <div>
    <el-row>
      <el-col :span="18">{{ location }}</el-col>
      <el-col :span="6">
        <el-dropdown size="small" @command="showInput">
          <el-button type="primary" size="small">{{ ('New') }}<i class="el-icon-arrow-down el-icon--right"></i></el-button>
          <el-dropdown-menu slot="dropdown">
              <el-dropdown-item command="newfile"><img style="vertical-align: text-bottom; margin-right: 2px" src="/icons/file.png" />{{ ('File') }}</el-dropdown-item>
              <el-dropdown-item command="newfolder"><img style="vertical-align: text-bottom; margin-right: 2px" src="/icons/folder.png" />{{ ('Folder') }}</el-dropdown-item>
          </el-dropdown-menu>
        </el-dropdown>
      </el-col>
    </el-row>
    <ul style="padding-left: 0; list-style-type: none" class="oui-filebrowser">
      <li @click="readdir('..')" style="cursor: pointer">
        <img style="vertical-align: bottom; margin-right: 2px" src="/icons/parent-folder.png" />
        <span>..</span>
      </li>
      <li v-for="f in files" :key="f.name" @click="readdir(f)" style="cursor: pointer" :class="{selected: selected === f.path}">
        <img style="vertical-align: bottom; margin-right: 2px" :src="f.dir ? '/icons/folder.png' : '/icons/file.png'" />
        <span>{{ f.name }}</span>
      </li>
      <li v-if="newType">
        <img style="vertical-align: bottom; margin-right: 2px" :src="(newType === 'file') ? '/icons/file.png' : '/icons/folder.png'" />
        <input ref="input" v-model="newFileName" @keyup.enter="newFile" @blur="newFile" />
      </li>
    </ul>
  </div>
</template>

<script>
export default {
  data() {
    return {
      location: '/',
      selected: '',
      files: [],
      newType: '',
      newFileName: ''
    }
  },
  methods: {
    readdir(file) {
      if (file === '..') {
        if (this.location === '/')
          return;
        let paths = this.location.spli('/');
        this.location = paths.slice(0, paths.length - 2).join('/') + '/';
      } else {
        if (!file.dir) {
          this.selected = file.path;
          this.$emit('selected', this.selected);
          return;
        }
        this.location = file.path + '/';
      }

      this.$ubus.call('oui.system', 'read_dir', {path: this.location}).then(r => {
        this.files = r.files;
      });
    },
    showInput(cmd) {
      if (cmd === 'newfile') {
        this.newType = 'file';
      } else {
        this.newType = 'folder';
      }

      this.$nextTick(() => {
        this.$refs['input'].focus();
      });
    },
    newFile() {
      const name = this.newFileName;
      const isDir = this.newType === 'folder';

      this.newFileName = '';
      this.newType = '';

      if (name) {
        for (let file of this.files) {
          if (file.name === name) {
            this.$message.warning(('Already exists'));
            return;
          }
        }

        const path = this.location + name;

        this.$ubus.call('oui.system', 'new_file', {path: path, dir: isDir}).then(r => {
          if (r.r === 0) {
            this.files.push({
              name: name,
              dir: isDir,
              path: path
            });

            if (!isDir) {
              this.selected = path;
              this.$emit('selected', this.selected);
            }
          }
        });
      }
    }
  },
  created() {
    this.$ubus.call('oui.system', 'read_dir').then(r => {
      this.files = r.files;
    });
  }
}
</script>

<style lang="scss">
.oui-filebrowser {
  li:hover {
    background-color: #CCE8FF80;
  }

  .selected {
    background-color: #CCE8FF;
  }
}
</style>
