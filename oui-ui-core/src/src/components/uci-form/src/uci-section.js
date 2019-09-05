import UciFormItem from './uci-form-item'
import UciSectionAdd from './uci-section-add'

export default {
  name: 'UciSection',
  provide() {
    return {
      uciSection: this
    };
  },
  inject: ['uciForm'],
  props: {
    /*
    ** The 'type' and 'name' must be provided at least one.
    ** If the name prop is provided, only the section whose name is equal to the name prop is rendered,
    ** otherwise render all sections whose type is equal to the type prop.
    */
    type: String,
    name: String,
    title: String,
    /* The type prop must be provided for add or delete */
    addable: Boolean,
    /* If false, the add operation will add a named section */
    anonymous: {
      type: Boolean,
      default: true
    },
    /* Render in a table */
    table: Boolean,
    /* Parameters: uci section, self */
    filter: Function,
    options: {
      type: Object,
      default: () => {}
    },
    /* Custom add function */
    add: Function,
    /* hook after add */
    afterAdd: Function,
    /* hook before del */
    beforeDel: Function,
    collabsible: {
      type: Boolean,
      default: true
    },
    teasers: Array,
    sortable: Boolean
  },
  components: {
    UciFormItem,
    UciSectionAdd
  },
  data() {
    return {
      loaded: false, /* Indicates whether the data is loaded */
      tabs: [], /* uci-tab vue instances */
      children: {}, /* uci-option vue instances */
      sections: [], /* uci sections */
      activeCollapseItem: ''
    }
  },
  computed: {
    config() {
      return this.uciForm.config;
    },
    tableExpandTabs() {
      return this.tabs.filter(tab => tab.isTableExpand);
    },
    arrayedOptions() {
      return Object.keys(this.children)
        .map(name => this.children[name])
        .sort((a, b) => a.uid - b.uid)
        .filter(o => !o.isTableExpand);
    },
    arrayedTableExpandOptions() {
      return Object.keys(this.children)
        .map(name => this.children[name])
        .sort((a, b) => a.uid - b.uid)
        .filter(o => o.isTableExpand);
    },
    sids() {
      let sections = [];

      if (this.name)
        sections = this.sections.filter(s => s['.name'] === this.name);
      else if (this.type)
        sections = this.sections;

      if (this.filter)
        sections = sections.filter(s => this.filter(s, this));
      return sections.map(s => s['.name']);
    },
    tableActionWidthCalc() {
      if (this.tableActionWidth)
        return this.tableActionWidth;
      if (this.sortable)
        return '150';
      return '70';
    }
  },
  watch: {
    'uciForm.loaded'() {
      this.load();
    }
  },
  methods: {
    formValue(name, sid) {
      const o = this.children[name];
      if (o)
        return o.formValue(sid);
      return undefined;
    },
    setFormValue(name, sid, val) {
      const o = this.children[name];
      if (o)
        o.setFormValue(sid, val);
    },
    addChild(o) {
      this.$set(this.children, o.name, o);
    },
    delChild(o) {
      const d = this.children[o.name];
      if (d && d.uid === o.uid)
        this.$delete(this.children, o.name);
    },
    load() {
      this.sections = this.$uci.sections(this.config, this.type);
      this.loaded = true;
    },
    buildForm(sid) {
      for (const name in this.children)
        this.children[name].buildForm(sid);
    },
    destroyForm(sid) {
      for (const name in this.children)
        this.children[name].destroyFormSid(sid);
    },
    loadAdded(sid) {
      this.activeCollapseItem = sid;
      this.load();
      this.buildForm(sid);
    },
    performDel(sid) {
      this.$uci.del(this.config, sid);
      this.load();
      this.destroyForm(sid);
    },
    del(sid) {
      if (this.beforeDel) {
        const before = this.beforeDel(sid, this);
        if (before === false)
          return;

        if (window.oui.isPromise(before)) {
          before.then(() => {
            this.performDel(sid);
          }, () => {
            /* ignore promise rejection */
          });
          return;
        }
      }
      this.performDel(sid);
    },
    swap(sid1, sid2) {
      this.$uci.swap(this.config, sid1, sid2);
      this.load();
    },
    teasersValue(sid) {
      const teasers = [];

      if (this.teasers) {
        this.teasers.forEach(name => {
          const o = this.children[name];
          teasers.push([o.label, o.textValue(sid)]);
        });
      } else {
        for (const name in this.children) {
          const o = this.children[name];
          const v = o.textValue(sid)
          teasers.push([o.label, v]);
        }
      }

      if (teasers.length > 0)
        teasers[teasers.length - 1].push('end');

      return teasers;
    },
    getErrorNum() {
      const validates = this.uciForm.validates;
      const keys = Object.keys(validates).filter(key => {
        const [sid] = key.split('_');
        return this.sids.indexOf(sid) > -1 && !validates[key].valid;
      });

      return keys.length;
    },
    save() {
      const promises = [];

      this.sids.forEach(sid => {
        for (const name in this.children) {
          const p = this.children[name]._save(sid);
          if (window.oui.isPromise(p))
            promises.push(p);
        }
      });

      return promises;
    },
    apply() {
      const promises = [];

      this.sids.forEach(sid => {
        for (const name in this.children) {
          const p = this.children[name]._apply(sid);
          if (window.oui.isPromise(p))
            promises.push(p);
        }
      });

      return promises;
    },
    applied() {
      this.sids.forEach(sid => {
        for (const name in this.children) {
          this.children[name].applied(sid);
        }
      });
    },
    handleSort(sid, up) {
      const sids = this.sids;
      let index = sids.indexOf(sid);

      if (up)
        index--;
      else
        index++;

      if (index < 0 || index === sids.length)
        return;

      const nsid = sids[index];
      this.swap(sid, nsid);
    },
    sectionView(sid, divider) {
      const views = [];

      if (this.addable && this.type && !this.name)
        views.push(<el-row><el-button style="float:right" type="danger" size="mini" on-click={this.del.bind(this, sid)}>{ this.$t('Delete') }</el-button></el-row>);

      if (this.tabs.length > 0) {
        const tabPanes = this.tabs.map(tab => {
          const errNum = tab.getErrorNum(sid);
          return (
            <el-tab-pane name={tab.name}>
              <span slot="label">{ tab.title }{errNum > 0 && <el-badge value={errNum} />}</span>
              { tab.options.map(o => <uci-form-item option={o} sid={sid} />) }
            </el-tab-pane>
          );
        });
        views.push(<el-tabs value={this.tabs[0].name}>{ tabPanes }</el-tabs>);
      }
      const noTabOptions = this.arrayedOptions.filter(o => !o.tabName);
      views.push(noTabOptions.map(o => <uci-form-item option={o} sid={sid} />));

      if (divider)
        views.push(<el-divider />);

      return views;
    },
    tableView() {
      const columns = [];

      /* If don't do this, an exception occurs when rendering the expandable table. */
      if (this.arrayedOptions.length < 1)
        return undefined;

      if (this.arrayedTableExpandOptions.length > 0) {
        const scopedSlots = {
          default: props => {
            const sid = props.row;
            const views = [];

            if (this.tableExpandTabs.length > 0) {
              const tabPanes = this.tableExpandTabs.map(tab => {
                const errNum = tab.getErrorNum(sid);
                return (
                  <el-tab-pane name={tab.name}>
                    <span slot="label">{ tab.title }{errNum > 0 && <el-badge value={errNum} />}</span>
                    { tab.options.map(o => <uci-form-item option={o} sid={sid} />) }
                  </el-tab-pane>
                );
              });
              views.push(<el-tabs value={this.tabs[0].name}>{ tabPanes }</el-tabs>);
            }

            const noTabOptions = this.arrayedTableExpandOptions.filter(o => !o.tabName);
            views.push(noTabOptions.map(o => <uci-form-item option={o} sid={sid} />));

            return views;
          }
        };

        columns.push(<el-table-column type="expand" scopedSlots={scopedSlots} />);
      }

      columns.push(...this.arrayedOptions.map(o => {
        const scopedSlots = {
          default: props => <uci-form-item option={o} sid={props.row} table={true} />
        };
        return <el-table-column label={o.label} width={o.width} scopedSlots={scopedSlots} />;
      }));

      let actionWidth = 0;
      if (this.addable && this.type && !this.name)
        actionWidth += 60;
      if (this.sortable)
        actionWidth += 100;

      if (actionWidth > 0) {
        const scopedSlots = {
          default: props => {
            const actions = [];

            if (this.sortable) {
              actions.push(<el-button type="success" size="mini" on-click={this.handleSort.bind(this, props.row, true)}>↑</el-button>);
              actions.push(<el-button type="success" size="mini" on-click={this.handleSort.bind(this, props.row, false)}>↓</el-button>);
            }

            if (this.addable && this.type && !this.name)
              actions.push(<el-button type="danger" size="mini" on-click={this.del.bind(this, props.row)}>{this.$t('Delete')}</el-button>);

            return <el-button-group style="margin-bottom: 22px">{ actions }</el-button-group>;
          }
        };
        columns.push(<el-table-column scopedSlots={scopedSlots} width={actionWidth + 25} />);
      }

      return <el-table data={this.sids}>{columns}</el-table>;
    },
    collapseView() {
      const items = this.sids.map(sid => {
        let title = [];

        if (sid !== this.activeCollapseItem) {
          const errNum = this.uciForm.getErrorNum(sid);
          if (errNum > 0)
            title.push(<el-badge value={errNum} />);

          const teasersValue = this.teasersValue(sid);
          const teasers = teasersValue.map((t, i) => {
            return `${t[0]}: <strong>${t[1]}</strong>${i < teasersValue.length - 1 ? ' | ' : ''}`;
          });
          title.push(<span domPropsInnerHTML={teasers.join('')}/>);
        }

        return (
          <el-collapse-item title={sid} name={sid}>
            <div slot="title">{title}</div>
            { this.sectionView(sid) }
          </el-collapse-item>
        );
      });

      return <el-collapse accordion v-model={this.activeCollapseItem}>{items}</el-collapse>;
    },
    view() {
      let view;

      if (this.table)
        view = this.tableView();
      else if (this.collabsible && this.sids.length > 1)
        view = this.collapseView();
      else
        view = this.sids.map(sid => this.sectionView(sid, this.addable));

      if (this.addable && this.type && !this.name)
        return [view, <uci-section-add sestion={this} />];
      else
        return view;
    }
  },
  created() {
    this.uciForm.sections.push(this);
    if (this.uciForm.loaded)
      this.load();
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
