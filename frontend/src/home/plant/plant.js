import "./style.css";
import "./nagprm.css";

export default function Plant({backgroundColor, onClickHandler}) {

    return (<>
        <div className={"plant_before"} style={{backgroundColor: backgroundColor}} onClick={onClickHandler}></div>
        <div className="plant" onClick={onClickHandler}>
            <div class="plant__leaves"></div>
        </div>
        </>
    )

}