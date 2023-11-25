<template>
  <el-space>
    <el-select v-model="group">
      <el-option v-for="o in groups" :key="o" :label="o" :value="o"/>
    </el-select>
    <el-button type="primary" @click="addGroup">{{ $t('Add group') }}</el-button>
    <el-button type="danger" @click="delGroup" :disabled="group == 'admin'">{{ $t('Delete group') }}</el-button>
  </el-space>
  <el-divider/>
  <el-table :data="acls[group]">
    <el-table-column prop="cls" :label="$t('Class')" width="80"/>
    <el-table-column :label="$t('Matchs')">
      <template #default="{ row }">
        <dynamic-tags v-model="row.matchs"/>
      </template>
    </el-table-column>
    <el-table-column :label="$t('Reverse')" width="80">
      <template #default="{ row }">
        <el-switch v-model="row.reverse"/>
      </template>
    </el-table-column>
  </el-table>
  <el-divider/>
  <el-button type="primary" @click="handleSubmit">{{ $t('Save & Apply') }}</el-button>
</template>

<script>
import DynamicTags from './dynamic-tags.vue'

export default {
  components: {
    DynamicTags
  },
  data() {
    return {
      group: '',
      acls: {}
    }
  },
  computed: {
    groups() {
      return Object.keys(this.acls)
    }
  },
  methods: {
    handleSubmit() {
      this.$oui.call('acl', 'set', { acls: this.acls })
    },
    addGroup() {
      this.$prompt('', this.$t('Add group'), {
      }).then(({ value }) => {
        value = value.trim()
        if (!value)
          return

        this.acls[value] = [ 'rpc', 'menu', 'ubus', 'uci' ].map(cls => {
          return {
            cls: cls,
            matchs: [ '.+' ],
            reverse: false
          }
        })

        this.group = value
      })
    },
    delGroup() {
      this.$confirm(this.$t('delete-group-confirm', { group: this.group }), this.$t('Delete group'), {
        type: 'warning'
      }).then(() => {
        delete this.acls[this.group]
        this.group = this.groups[0]
      })
    }
  },
  created() {
    this.$oui.call('acl', 'load').then(acls => {
      this.acls = acls
      this.group = this.groups[0]
    })
  }
}
</script>

<i18n src="./locale.json"/>
