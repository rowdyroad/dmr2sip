import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import PageLocation from '../../utils/PageLocation';
import * as Actions from '../../actions';
import {Router, browserHistory, Link} from 'react-router'
import * as UI from '../../components/UIKit'

class Item extends Component
{
    componentWillMount = () =>{
        this.props.actions.fetch(this.props.params.point_id)
    }

    componentWillUpdate = (next) => {
      if (next.data) {
        next.route.name = next.data.name;
        next.actions.setState({title: next.data.name});
      }

      if ((!next.context && this.props.context) || (next.context && next.context.error == 404)) {
          this.props.actions.notFound();
      }
    }
    render = () => {

        if (!this.props.data) {
            return null;
        }

        return (
                    <div>
                        <UI.Modal isOpen={this.props.context.showRemoveModal} title="Removing current item" onClose={this.props.actions.hideRemoveModal}>
                            <div style={{padding:'1em'}}>Do you really want to remove current element <b>{this.props.data.name}</b>?</div>
                            <UI.Button label="Remove" color="danger" onClick={()=>{ this.props.actions.delete(this.props.data)}}/>
                            &nbsp;
                            <UI.Button label="Cancel" color="white" onClick={this.props.actions.hideRemoveModal}/>
                        </UI.Modal>
                        {React.Children.map(this.props.children, (child)=> {
                          return React.cloneElement(child, this.props)
                        })}
                    </div>
                );
    }
}



let mapState = (state, props) => {
    return {
                data: state.main.getIn(['point','success']) ? state.main.getIn(['point','response']).toJS() : null,
                context: state.main.has('point') ? state.main.get('point').toJS() : null,
            }
}

let mapDispatch = (dispatch) => {
  return {
    actions: {
      fetch: (id) => {
        dispatch(Actions.Fetch("point","/api/points/" + id));
      },
      delete:(data) => {
        dispatch(Actions.Delete("point", "/api/points/" + data.point_id, {}, {
            remove:true,
            onSuccess:Actions.ListItemRemove(["points","response","data"], 'point_id', data.point_id)
        }));
      },
      setState:(state) => {
        dispatch(Actions.ObjectMerge("point", state))
      },
      showRemoveModal:() => {
        dispatch(Actions.ObjectMerge("point", {showRemoveModal:true}));
      },
      hideRemoveModal:()=>{
        dispatch(Actions.ObjectMerge("point", {showRemoveModal:false}));
      },
      close: () => {
        browserHistory.goBack();
      },
      notFound: () => {
        browserHistory.replace('/points');
      }
    }
  }
}

export default connect(mapState, mapDispatch)(Item)