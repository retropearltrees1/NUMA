interface ChatHeaderProps {
  messageCount: number;
  onClear: () => void;
}

export default function ChatHeader({ messageCount, onClear }: ChatHeaderProps) {
  return (
    <header className="chat-header">
      <div className="chat-header-left">
        <div className="chat-header-logo">N</div>
        <div className="chat-header-text">
          <span className="chat-header-title">NUMA</span>
          <span className="chat-header-status">
            <span className="status-dot" />
            Hors-ligne · {messageCount} message{messageCount !== 1 ? "s" : ""}
          </span>
        </div>
      </div>
      {messageCount > 0 && (
        <button className="clear-btn" onClick={onClear} title="Effacer la conversation">
          Effacer
        </button>
      )}
    </header>
  );
}
