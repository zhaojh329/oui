<template>
  <component :is="container" :title="title">
    <a-table v-if="columns" :columns="tableColumns" :data-source="dataSource">
      <template v-if="title" #title>
        <span style="font-size: 16px; font-weight: 500; color: rgba(0, 0, 0, 0.85)">{{ title }}</span>
      </template>
      <template v-for="c in columns" v-slot:[c.name]="text,record">
        <slot :name="c.name" :s="record"/>
      </template>
      <template v-for="c in columns.filter(c => c.titleSlot)" v-slot:[c.titleSlot]>
        <slot :name="c.titleSlot"/>
      </template>
      <template v-slot:oui-form-table-action="sid">
        <a-button v-if="sortable" size="small" type="primary" style="margin-right: 5px" @click="handleSort(sid, -1)" icon="arrow-up"/>
        <a-button v-if="sortable" size="small" type="primary" style="margin-right: 5px" @click="handleSort(sid, 1)" icon="arrow-down"/>
        <a-button v-if="addremove" size="small" type="danger" @click="delSection(sid)">{{ $t('Delete') }}</a-button>
      </template>
      <template v-if="expanded" #expandedRowRender="record">
        <slot name="expandedRowRender" :s="record"/>
      </template>
    </a-table>
    <template v-else-if="collapsible">
      <a-collapse v-model="collapse" :accordion="accordion">
        <a-collapse-panel v-for="s in sections" :key="s['.name']">
          <template #header>
            <span>{{ renderTeasers(s['.name']) }}</span>
          </template>
          <template #extra>
            <a-tooltip :title="$t('Delete')">
              <a-icon type="delete" style="color: red" @click.prevent="delSection(s['.name'])" />
            </a-tooltip>
          </template>
          <slot :s="s"/>
        </a-collapse-panel>
      </a-collapse>
    </template>
    <template v-else>
      <div v-for="(s, i) in sections" :key="s['.name']">
        <oui-typed-section-body :sid="s['.name']">
          <slot :s="s"/>
        </oui-typed-section-body>
        <a-divider v-if="i < sections.length - 1"/>
      </div>
    </template>
    <a-button v-if="addremove" type="primary" size="small" style="margin-top: 10px" @click="handleAdd">{{ $t('Add') }}</a-button>
  </component>
</template>

<script>
import OuiSection from './OuiSection.vue'
import OuiTypedSectionBody from './OuiTypedSectionBody.vue'

export default {
  name: 'OuiTypedSection',
  mixins: [OuiSection],
  props: {
    type: {
      type: String,
      required: true
    },
    columns: Array,
    anonymous: {
      type: Boolean,
      default: true
    },
    addremove: Boolean,
    collapsible: {
      type: Boolean,
      default: true
    },
    teasers: Array,
    add: Function,
    sortable: Boolean,
    expanded: Boolean,
    filter: Function
  },
  components: {
    OuiTypedSectionBody
  },
  data () {
    return {
      accordion: false,
      collapse: [],
      sections: []
    }
  },
  computed: {
    dataSource () {
      return this.sections.map((v, i) => {
        v.key = i
        return v
      })
    },
    tableColumns () {
      const columns = []
      this.columns.forEach(c => {
        columns.push({
          dataIndex: c.name,
          title: c.label,
          slots: { title: c.titleSlot },
          width: c.width,
          scopedSlots: { customRender: c.name }
        })
      })

      let actionWidth = 0
      if (this.addremove) actionWidth += 60
      if (this.sortable) actionWidth += 80

      if (actionWidth > 0) {
        columns.push({
          dataIndex: '.name',
          width: actionWidth,
          scopedSlots: { customRender: 'oui-form-table-action' }
        })
      }

      return columns
    }
  },
  methods: {
    updateCollapse (sid) {
      if (this.collapsible) {
        this.collapse = sid
      }
    },
    renderTeasers (sid) {
      return Object.values(this.ouiForm.fields).filter(f => {
        if (this.teasers && this.teasers.indexOf(f.name) === -1) return false
        return f.sid === sid
      }).map(f => {
        let value = f.model
        if (typeof value === 'undefined') { value = '' }
        return `${f.label}: ${value}`
      }).join('| ')
    },
    handleSort (sid, pos) {
      const sids = this.sections.map(s => s['.name'])
      let index = sids.indexOf(sid)

      index += pos

      if (index < 0 || index === sids.length) return

      const nsid = sids[index]

      this.$uci.swap(this.config, sid, nsid)
      this.load()
    },
    delSection (sid) {
      this.$uci.del(this.config, sid)
      this.load()
      if (this.collapsible && this.sections.length > 0) {
        this.collapse = this.sections[0]['.name']
      } else {
        this.collapse = null
      }
    },
    addSectionPost (sid) {
      this.load()
      this.updateCollapse(sid)
    },
    addSection (name) {
      const sid = this.$uci.add(this.config, this.type, name)
      return sid
    },
    handleAdd () {
      if (this.add) {
        this.addSectionPost(this.add(this))
        return
      }

      if (this.anonymous) {
        this.addSectionPost(this.addSection())
        return
      }

      this.$prompt({
        title: this.$t('Add'),
        placeholder: this.$t('Please input a name'),
        validator: value => {
          if (value.match(/^[a-zA-Z0-9_]+$/) === null) {
            return this.$t('validator.uci')
          }

          if (this.sections.filter(s => s['.name'] === value).length > 0) {
            return this.$t('Name already used')
          }
        }
      }).then(value => {
        this.addSectionPost(this.addSection())
      }).catch(() => {})
    },
    load () {
      this.sections = this.$uci.sections(this.config, this.type).filter(s => this.filter ? this.filter(s) : true)
    }
  },
  watch: {
    loaded () {
      this.load()
    }
  },
  mounted () {
    if (this.collapsible && this.sections.length > 0) {
      this.collapse = this.sections.map(s => s['.name'])
      this.$nextTick(() => {
        this.accordion = true
      })
    }
  }
}
</script>
